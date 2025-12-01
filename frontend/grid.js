document.addEventListener("DOMContentLoaded", initButtons);

class Grid
{
  constructor(containerElement) 
  {
    this.container = containerElement;
    this.items = [];
    this.backendURL = "http://localhost:8080/result";
    this.renderer = new GridRenderer(this.container);
    this.pointerAnimChain = Promise.resolve();
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
    this.renderer.render(this.items);

    const children = this.container.querySelectorAll("." + CSS_CLASS.gridItem);
    const topElement = children[0] || null;
    return topElement;
  }

  push(text)
  {
    const topElement = this._pushCore(text);
    if (!topElement) return;

    // Add animation to the new item
    topElement.classList.add(CSS_CLASS.pushAnim);
    topElement.addEventListener(
      "animationend",
      () =>
      {
        topElement.classList.remove(CSS_CLASS.pushAnim);
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
      topElement.classList.add(CSS_CLASS.pushAnim);

      topElement.addEventListener(
        "animationend",
        () =>
        {
          topElement.classList.remove(CSS_CLASS.pushAnim);
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
      this.renderer.render(this.items);
      return;
    }

    const children = this.container.querySelectorAll("." + CSS_CLASS.gridItem);
    const lastItemElement = children[0];

    lastItemElement.classList.add(CSS_CLASS.popAnim);

    lastItemElement.addEventListener(
      "animationend",
      () =>
      {
        this.items.pop();
        this.renderer.render(this.items);
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
    this.pointerAnimChain = this.pointerAnimChain.then(() =>
    {
      return this._moveTopPointerOneStep();
    });
  }

  _moveTopPointerOneStep()
  {
    return new Promise((resolve) =>
    {
      // No items
      if (this.items.length === 0)
      {
        resolve();
        return;
      }

      // No words in top item
      const topItem = this.items[this.items.length - 1];
      if (!topItem || topItem.words.length === 0)
      {
        resolve();
        return;
      }

      // Record previous pointer, update pointer
      topItem.prevPointer = topItem.pointer;
      topItem.pointer = topItem.pointer + 1;

      // Store this time's animation resolver for render to call later
      this.renderer.setPointerAnimResolver(resolve);
      this.renderer.render(this.items);
    });
  }

}

function initButtons()
{
  const gridContainer = document.getElementById(CSS_CLASS.gridContainer);
  const input = document.getElementById("item-input");
  const pushButton = document.getElementById("push-button")
  const popButton = document.getElementById("pop-button")
  const loadButton = document.getElementById("load-button");
  const nextWordButton = document.getElementById("next-word-button");


  const grid = new Grid(gridContainer);
  grid.renderer.render(grid.items);

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

  debug(grid);
}

async function debug(grid)
{
  // Test loading from server
  await grid.loadTextFromServer();
}