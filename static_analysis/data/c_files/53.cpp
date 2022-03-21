void InspectorOverlay::update()
{
if (isEmpty()) {
m_client->hideHighlight();
return;
}

FrameView* view = m_page->mainFrame()->view();
if (!view)
return;
IntRect viewRect = view->visibleContentRect();
FrameView* overlayView = overlayPage()->mainFrame()->view();

// Include scrollbars to avoid masking them by the gutter.
IntSize frameViewFullSize = view->visibleContentRect(ScrollableArea::IncludeScrollbars).size();
IntSize size = m_size.isEmpty() ? frameViewFullSize : m_size;
size.scale(m_page->pageScaleFactor());
overlayView->resize(size);

// Clear canvas and paint things.
reset(size, m_size.isEmpty() ? IntSize() : frameViewFullSize, viewRect.x(), viewRect.y());

drawGutter();
drawNodeHighlight();
drawQuadHighlight();
if (!m_inspectModeEnabled)
drawPausedInDebuggerMessage();
drawViewSize();
    drawOverridesMessage();

// Position DOM elements.
overlayPage()->mainFrame()->document()->recalcStyle(Force);
if (overlayView->needsLayout())
overlayView->layout();

// Kick paint.
m_client->highlight();
}
