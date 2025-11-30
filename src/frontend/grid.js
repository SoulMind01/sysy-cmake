document.addEventListener("DOMContentLoaded", initButtons);


class Grid
{
  constructor(containerElement) 
  {
    this.container = containerElement;
    this.items = [];
    this.previousTopIndex = 1;
    this.backendURL = "http://localhost:8080/result";
  }

  render()
  {
    this.container.innerHTML = "";

    if (this.items.length === 0)
    {
      this.container.textContent = "Grid is empty.";
      return;
    }

    const currentTopIndex = 0;

    // Reverse order
    this.items.slice().reverse().forEach((item, index) =>
    {
      const itemDiv = document.createElement("div");
      itemDiv.className = "grid-item";

      // 1. Handle whether it's the top item
      if (index === currentTopIndex)
      {
        itemDiv.classList.add("grid-item-top");
        itemDiv.classList.add("grid-item-top-enter");
        itemDiv.addEventListener(
          "animationend",
          () =>
          {
            itemDiv.classList.remove("grid-item-top-enter");
          },
          { once: true }
        );
      }
      // If it's leaving the top position
      else if (index === this.previousTopIndex)
      {
        itemDiv.classList.add("grid-item-top-leave");
        itemDiv.addEventListener(
          "animationend",
          () =>
          {
            itemDiv.classList.remove("grid-item-top-leave");
          },
          { once: true }
        );
      }

      // 2. Render words
      const wordsDiv = document.createElement("div");
      wordsDiv.className = "grid-item-words";

      item.words.forEach((word, wIndex) =>
      {
        const span = document.createElement("span");
        span.className = "grid-word";

        // Static highlight
        if (wIndex === item.pointer)
        {
          span.classList.add("grid-word-active");
          // If previous pointer is different, this word is newly activated. Add enter animation
          if (item.prevPointer !== item.pointer)
          {
            span.classList.add("grid-word-active-enter")
            span.addEventListener(
              "animationend",
              () =>
              {
                span.classList.remove("grid-word-active-enter");
              },
              { once: true }
            );
          }
        }
        // Handle leaving animation
        else if (item.prevPointer === wIndex)
        {
          span.classList.add("grid-word-active-leave");
          span.addEventListener(
            "animationend",
            () =>
            {
              span.classList.remove("grid-word-active-leave");
            },
            { once: true }
          );
        }

        span.textContent = word;
        wordsDiv.appendChild(span);
      });

      itemDiv.appendChild(wordsDiv);
      this.container.appendChild(itemDiv);
    });

    // Update previous pointer
    this.items.forEach(item =>
    {
      item.prevPointer = item.pointer;
    });
  }


  _pushCore(text)
  {
    if (text === "")
    {
      alert("Please enter text.");
      return null;
    }

    const indentMatch = text.match(/^\s*/);
    const leadingIndent = indentMatch ? indentMatch[0] : "";

    const words = this.splitIntoWords(text);

    if (leadingIndent.length > 0 && words.length > 0)
    {
      words[0] = leadingIndent + words[0];
    }

    const item = {
      raw: text,
      words: words,
      pointer: 0,
      prevPointer: null
    };

    this.items.push(item);
    this.render();

    const children = this.container.querySelectorAll(".grid-item");
    const topElement = children[0] || null;
    return topElement;
  }

  push(text)
  {
    const topElement = this._pushCore(text);
    if (!topElement) return;

    // Add animation to the new item
    topElement.classList.add("push-anim");
    topElement.addEventListener(
      "animationend",
      () =>
      {
        topElement.classList.remove("push-anim");
      },
      { once: true }
    );
  }

  async pushSequential(text)
  {
    const topElement = this._pushCore(text);
    if (!topElement) return;

    await new Promise((resolve) =>
    {
      topElement.classList.add("push-anim");

      topElement.addEventListener(
        "animationend",
        () =>
        {
          topElement.classList.remove("push-anim");
          resolve();
        },
        { once: true }
      );
    });
  }

  pop()
  {
    if (this.items.length === 0)
    {
      this.render();
      return;
    }

    const children = this.container.querySelectorAll(".grid-item");
    const lastItemElement = children[0];

    lastItemElement.classList.add("pop-anim");

    lastItemElement.addEventListener(
      "animationend",
      () =>
      {
        this.items.pop();
        this.render();
      },
      { once: true }
    )
  }

  splitIntoWords(text)
  {
    return text
      .split(/\s+/)
      .map(w => w.trim())
      .filter(w => w.length > 0);
  }

  async loadTextFromServer()
  {
    try
    {
      const response = await fetch(this.backendURL);
      if (!response.ok)
      {
        console.error("Fetch error:", response.statusText);
        return;
      }

      const text = await response.text();

      const lines = text.trim().split(/\r?\n/);

      const reversed = lines.slice().reverse();
      for (const line of reversed)
      {
        if (line === "") continue;
        await this.pushSequential(line);
      }
    }
    catch (error)
    {
      console.error("Error fetching data from server:", error);
    }
  }

  moveTopPointerNext()
  {
    if (this.items.length === 0) return;

    const topItem = this.items[this.items.length - 1];
    if (!topItem || topItem.words.length === 0) return;

    topItem.prevPointer = topItem.pointer;
    topItem.pointer = topItem.pointer + 1;

    this.render();
  }

}

function initButtons()
{
  const gridContainer = document.getElementById("grid-container");
  const input = document.getElementById("item-input");
  const pushButton = document.getElementById("push-button")
  const popButton = document.getElementById("pop-button")
  const loadButton = document.getElementById("load-button");
  const nextWordButton = document.getElementById("next-word-button");


  const grid = new Grid(gridContainer);
  grid.render();

  pushButton.addEventListener("click", () =>
  {
    const value = input.value.trim();
    grid.push(value);
    input.value = "";
    input.focus();
  });

  popButton.addEventListener("click", () =>
  {
    grid.pop();
  });

  loadButton.addEventListener("click", () =>
  {
    grid.loadTextFromServer();
  });

  nextWordButton.addEventListener("click", () =>
  {
    grid.moveTopPointerNext();
  });

}