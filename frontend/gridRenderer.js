class GridRenderer
{
  constructor(containerElement)
  {
    this.container = containerElement;
    this.prevTopIndex = null;
    this.pointerAnimResolver = null;
  }

  setPointerAnimResolver(resolver)
  {
    this.pointerAnimResolver = resolver;
  }

  render(items)
  {
    this.container.innerHTML = "";

    if (items.length === 0)
    {
      this.container.textContent = "Grid is empty.";
      this.prevTopIndex = null;
      return;
    }

    const currentTopIndex = 0;

    items.slice().reverse().forEach((item, index) =>
    {
      const itemDiv = document.createElement("div");
      itemDiv.className = CSS_CLASS.gridItem;

      // 1. Highlight top item & top-enter/top-leave animations
      if (index === currentTopIndex)
      {
        itemDiv.classList.add(CSS_CLASS.gridItemTop);
        itemDiv.classList.add(CSS_CLASS.gridItemTopEnter);
        itemDiv.addEventListener(
          "animationend",
          () =>
          {
            itemDiv.classList.remove(CSS_CLASS.gridItemTopEnter);
          },
          { once: true }
        );
      }
      else if (index === this.prevTopIndex)
      {
        itemDiv.classList.add(CSS_CLASS.gridItemTopLeave);
        itemDiv.addEventListener(
          "animationend",
          () =>
          {
            itemDiv.classList.remove(CSS_CLASS.gridItemTopLeave);
          },
          { once: true }
        );
      }
      // 2. Render word & pointer highlight animation
      const wordsDiv = document.createElement("div");
      wordsDiv.className = CSS_CLASS.gridItemWords;

      item.words.forEach((word, wIndex) =>
      {
        const span = document.createElement("span");
        span.className = "grid-word";

        if (wIndex === item.pointer) 
        {
          span.classList.add(CSS_CLASS.gridWordActive);
          if (item.prevPointer !== item.pointer)
          {
            span.classList.add(CSS_CLASS.gridWordActiveEnter);
            span.addEventListener("animationend", () =>
            {
              span.classList.remove(CSS_CLASS.gridWordActiveEnter);
              if (this.pointerAnimResolver)
              {
                this.pointerAnimResolver();
                this.pointerAnimResolver = null;
              }
            },
              { once: true });
          }
        }
        else if (item.prevPointer === wIndex)
        {
          span.classList.add(CSS_CLASS.gridWordActiveLeave);
          span.addEventListener("animationend", () =>
          {
            span.classList.remove(CSS_CLASS.gridWordActiveLeave);
          }, { once: true });
        }

        span.textContent = word;
        wordsDiv.appendChild(span);
      });

      itemDiv.appendChild(wordsDiv);
      this.container.appendChild(itemDiv);
    });

    // 3. Update previous top index / previous pointer
    items.forEach(item =>
    {
      item.prevPointer = item.pointer;
    });

    this.prevTopIndex = currentTopIndex;
  }
}