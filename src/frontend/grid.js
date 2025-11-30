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
            console.log("Top item enter animation ended");
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

        // Highlight the pointing word
        if (wIndex === item.pointer)
        {
          span.classList.add("grid-word-active");
        }

        span.textContent = word;
        wordsDiv.appendChild(span);
      });

      itemDiv.appendChild(wordsDiv);
      this.container.appendChild(itemDiv);
    });

  }


  _pushCore(text)
  {
    if (text === "")
    {
      alert("Please enter text.");
      return null;
    }

    const words = this.splitIntoWords(text);
    const item = {
      raw: text,
      words: words,
      pointer: 0
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
        console.log("Push animation ended");
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
}

function initButtons()
{
  const gridContainer = document.getElementById("grid-container");
  const input = document.getElementById("item-input");
  const pushButton = document.getElementById("push-button")
  const popButton = document.getElementById("pop-button")
  const loadButton = document.getElementById("load-button");


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

}