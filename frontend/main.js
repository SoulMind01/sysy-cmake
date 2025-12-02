document.addEventListener("DOMContentLoaded", main);
function main()
{
  const visual = new VisualLinker();

  const gridContainer = document.getElementById(CSS_CLASS.gridContainer);

  const testButton = document.getElementById("test-visual-button");
  const eraseButton = document.getElementById("erase-one");

  if (testButton)
  {
    testButton.addEventListener("click", async () =>
    {
      const items = gridContainer.querySelectorAll("." + CSS_CLASS.gridItem);
      if (items.length >= 2)
      {
        const fromEl = items[0];
        const toEl = items[1];
        await visual.animateFlow(fromEl, toEl);
      }
    });
  }

  if (eraseButton)
  {
    eraseButton.addEventListener("click", () =>
    {
      visual.eraseNextShape();
    });
  }
}