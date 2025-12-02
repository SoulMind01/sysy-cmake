document.addEventListener("DOMContentLoaded", main);
function main()
{
  const visual = new VisualLinker();
  const testButton = document.getElementById("test-visual-button");
  const gridContainer = document.getElementById(CSS_CLASS.gridContainer);
  if (testButton)
  {
    testButton.addEventListener("click", async () =>
    {
      const items = gridContainer.querySelectorAll("." + CSS_CLASS.gridItem);
      console.log("Items:", items);
      if (items.length >= 2)
      {
        const fromEl = items[0];
        const toEl = items[1];
        await visual.animateFlow(fromEl, toEl);
      }
    });
  }
}