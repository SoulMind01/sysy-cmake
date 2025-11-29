document.addEventListener("DOMContentLoaded", initButtons);


class Grid
{
  constructor(containerElement, wrapperElement) 
  {
    this.container = containerElement;
    this.items = [];
    this.previousTopIndex = 1;
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

  push(text)
  {
    if (text === "")
    {
      alert("Please enter text.");
      return;
    }

    const words = this.splitIntoWords(text);
    console.log(words);

    const item = {
      raw: text,
      words: words,
      pointer: 0
    };

    this.items.push(item);
    this.render();

    // Add animation to the new item
    const children = this.container.querySelectorAll(".grid-item");
    const lastItemElement = children[0];
    if (!lastItemElement) return;

    lastItemElement.classList.add("push-anim");

    lastItemElement.addEventListener(
      "animationend",
      () =>
      {
        lastItemElement.classList.remove("push-anim");
      },
      { once: true }
    );
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
}

function initButtons()
{
  const gridContainer = document.getElementById("grid-container");
  const input = document.getElementById("item-input");
  const pushButton = document.getElementById("push-button")
  const popButton = document.getElementById("pop-button")


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

}