class VisualLinker
{
  constructor()
  {
    // Create SVG overlay layer
    this.svg = document.createElementNS("http://www.w3.org/2000/svg", "svg");
    this.svg.classList.add(CSS_CLASS.vizOverlay);

    // Define arrow head marker
    this._initDefs();

    document.body.appendChild(this.svg);

    // Store all active shapes
    this.activeShapes = [];

    // Listen to window resize
    window.addEventListener("resize", () => this._updateActiveShapes());
    window.addEventListener("scroll", () => this._updateActiveShapes(), true);
  }

  _registerOutlineShape(el, path, padding)
  {
    this.activeShapes.push({
      type: "outline",
      element: el,
      svgElement: path,
      padding
    });
  }

  _registerArrowShape(fromEl, toEl, path)
  {
    this.activeShapes.push({
      type: "arrow",
      fromEl,
      toEl,
      svgElement: path
    });
  }

  _updateActiveShapes()
  {
    for (const shape of this.activeShapes)
    {
      if (shape.type === "outline")
      {
        this._recomputeOutline(shape);
      }
      else if (shape.type === "arrow")
      {
        this._recomputeArrow(shape);
      }
    }
  }

  _recomputeOutline(shape)
  {
    const el = shape.element;
    const path = shape.svgElement;
    const padding = shape.padding;

    if (!el || !path) return;

    const rect = el.getBoundingClientRect();

    const left = rect.left - padding;
    const top = rect.top - padding;
    const right = rect.left + rect.width + padding;
    const bottom = rect.top + rect.height + padding;

    const topMidX = (left + right) / 2;
    const topMidY = top;

    const d = [
      `M ${topMidX} ${topMidY}`,
      `L ${right} ${topMidY}`,
      `L ${right} ${bottom}`,
      `L ${left} ${bottom}`,
      `L ${left} ${topMidY}`,
      `L ${topMidX} ${topMidY}`
    ].join(" ");

    path.setAttribute("d", d);
  }

  _recomputeArrow(shape)
  {
    const fromEl = shape.fromEl;
    const toEl = shape.toEl;
    const path = shape.svgElement;

    if (!fromEl || !toEl || !path) return;

    const r1 = fromEl.getBoundingClientRect();
    const r2 = toEl.getBoundingClientRect();

    const startX = r1.right;
    const startY = r1.top + r1.height / 2;

    const endX = r2.left;
    const endY = r2.top + r2.height / 2;

    const offset = Math.max(40, Math.min(120, Math.abs(endX - startX) / 3));

    const p1x = startX + offset;
    const p1y = startY;

    const p4x = endX - offset;
    const p4y = endY;

    const midY = (startY + endY) / 2;
    const c1x = p1x;
    const c1y = midY;
    const c2x = p4x;
    const c2y = midY;

    const d = [
      `M ${startX} ${startY}`,
      `L ${p1x} ${p1y}`,
      `C ${c1x} ${c1y} ${c2x} ${c2y} ${endX} ${endY}`
    ].join(" ");

    path.setAttribute("d", d);
  }

  _initDefs()
  {
    const defs = document.createElementNS("http://www.w3.org/2000/svg", "defs");
    const marker = document.createElementNS("http://www.w3.org/2000/svg", "marker");
    marker.setAttribute("id", "viz-arrow-head");
    marker.setAttribute("markerWidth", "10");
    marker.setAttribute("markerHeight", "10");
    marker.setAttribute("refX", "8");
    marker.setAttribute("refY", "5");
    marker.setAttribute("orient", "auto");
    marker.setAttribute("markerUnits", "strokeWidth");

    // Arrow head shape definition
    const arrowHead = document.createElementNS("http://www.w3.org/2000/svg", "path");
    arrowHead.setAttribute("d", "M 0 0 L 10 5 L 0 10 z");
    arrowHead.setAttribute("fill", "#42a5f5");

    marker.appendChild(arrowHead);
    defs.appendChild(marker);
    this.svg.appendChild(defs);
  }

  // Helper: create a SVG element
  _createSvgElement(tag)
  {
    return document.createElementNS("http://www.w3.org/2000/svg", tag);
  }

  // Draw a circle around the element. Return a promise. When the animation ends, the promise resolves
  highlightElement(el)
  {
    return new Promise((resolve) =>
    {
      if (!el)
      {
        resolve();
        return;
      }

      const rect = el.getBoundingClientRect();
      const centerX = rect.left + rect.width / 2;
      const centerY = rect.top + rect.height / 2;

      const radius = Math.max(rect.width, rect.height) * 0.6;

      const circle = this._createSvgElement("circle");
      circle.setAttribute("cx", centerX);
      circle.setAttribute("cy", centerY);
      circle.setAttribute("r", radius);
      circle.classList.add("viz-circle");

      this.svg.appendChild(circle);

      circle.addEventListener(
        "animationend",
        () =>
        {
          resolve();
        },
        { once: true }
      );
    });
  }

  /**
   * Draw a highlighted line around the element
   * Starting from the midpoint of the top edge
   */
  highlightBox(el, padding = 0.6)
  {
    return new Promise((resolve) =>
    {
      if (!el)
      {
        resolve();
        return;
      }
      const rect = el.getBoundingClientRect();
      const left = rect.left - padding;
      const top = rect.top - padding;
      const right = rect.right + padding;
      const bottom = rect.bottom + padding;

      const topMidX = (left + right) / 2;
      const topMidY = top;

      // M: move to, L: line to
      const d = [
        `M ${topMidX} ${topMidY}`, // Top mid
        `L ${right} ${topMidY}`, // Top right
        `L ${right} ${bottom}`, // Bottom right
        `L ${left} ${bottom}`, // Bottom left
        `L ${left} ${topMidY}`, // Top left
        `L ${topMidX} ${topMidY}` // Back to top mid
      ].join(" ");

      const path = this._createSvgElement("path");
      path.setAttribute("d", d);
      path.classList.add(CSS_CLASS.vizOutlinePath);
      this.svg.appendChild(path);

      // Set dasharray with path length
      const totalLen = path.getTotalLength();

      path.style.setProperty("--path-len", totalLen);

      path.addEventListener(
        "animationend",
        () =>
        {
          // TODO: remove path after animation
          resolve();
        },
        { once: true }
      );
      this._registerOutlineShape(el, path, padding);
    });
  }

  // Draw
  drawArrowBetween(fromEl, toEl)
  {
    return new Promise((resolve) =>
    {
      if (!fromEl || !toEl)
      {
        resolve();
        return;
      }

      const r1 = fromEl.getBoundingClientRect();
      const r2 = toEl.getBoundingClientRect();

      const startX = r1.right;
      const startY = r1.top + r1.height / 2;

      const endX = r2.left;
      const endY = r2.top + r2.height / 2;

      // In order to make the arrow curve, we first draw a horizontal line, then connect with cubic Bezier curve
      const offset = Math.max(40, Math.min(120, Math.abs(endX - startX) / 3));

      // Extends towards right
      const p1x = startX + offset;
      const p1y = startY;

      // Extends towards left when approaching target
      const p4x = endX - offset;
      const p4y = endY;

      // Control point: When approaching p1/p4, start banding vertically in order to create curve
      const midY = (startY + endY) / 2;
      const c1x = p1x;
      const c1y = midY;
      const c2x = p4x;
      const c2y = midY;

      // C for cubic Bezier curve
      const d = [
        `M ${startX} ${startY}`, // Start: Mid of right edge
        `L ${p1x} ${p1y}`, // Move right
        `C ${c1x} ${c1y} ${c2x} ${c2y} ${endX} ${endY}`
      ].join(" ");

      const path = this._createSvgElement("path");
      path.setAttribute("d", d);
      path.classList.add(CSS_CLASS.vizArrowPath);
      path.setAttribute("marker-end", "url(#viz-arrow-head)");

      this.svg.appendChild(path);

      // Set real arrow length
      const totalLen = path.getTotalLength();
      path.style.setProperty("--path-len", totalLen);

      // Resolve when animation ends
      path.addEventListener(
        "animationend",
        () =>
        {
          resolve();
        },
        { once: true }
      );

      // Register for later resize
      this._registerArrowShape(fromEl, toEl, path);
    });
  }

  async animateFlow(fromEl, toEl)
  {
    await this.highlightBox(fromEl);
    await this.drawArrowBetween(fromEl, toEl);
    await this.highlightBox(toEl);
  }
}

window.VisualLinker = VisualLinker;