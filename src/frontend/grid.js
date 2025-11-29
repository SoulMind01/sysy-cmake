document.addEventListener("DOMContentLoaded", initButtons);


class Grid
{
  constructor(containerElement) 
  {
    this.container = containerElement;
    this.items = [];
  }

  render()
  {
    this.container.innerHTML = "";

    // Reverse order
    this.items.slice().reverse().forEach((text, index) =>
    {
      const itemDiv = document.createElement("div");
      itemDiv.className = "grid-item";
      itemDiv.textContent = text;
      this.container.appendChild(itemDiv);
    });

    if (this.items.length === 0)
    {
      this.container.textContent = "Grid is empty.";
    }
  }

  push(itemText)
  {
    if (itemText === "")
    {
      alert("Please enter text.");
      return;
    }
    this.items.push(itemText);
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
  })
}