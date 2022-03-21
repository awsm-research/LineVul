void PaintLayerScrollableArea::UpdateCompositingLayersAfterScroll() {
PaintLayerCompositor* compositor = GetLayoutBox()->View()->Compositor();
if (!compositor->InCompositingMode())
return;

if (UsesCompositedScrolling()) {
DCHECK(Layer()->HasCompositedLayerMapping());
ScrollingCoordinator* scrolling_coordinator = GetScrollingCoordinator();
bool handled_scroll =
        Layer()->IsRootLayer() && scrolling_coordinator &&
scrolling_coordinator->UpdateCompositedScrollOffset(this);

if (!handled_scroll) {
if (!RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) {
// In non-BGPT mode, we need to do a full sub-tree update here, because
// we need to update the position property to compute
// offset_to_transform_parent. For more context, see the comment from
// chrishtr@ here:
// https://chromium-review.googlesource.com/c/chromium/src/+/1403639/6/third_party/blink/renderer/core/paint/paint_layer_scrollable_area.cc
Layer()->GetCompositedLayerMapping()->SetNeedsGraphicsLayerUpdate(
kGraphicsLayerUpdateSubtree);
}
compositor->SetNeedsCompositingUpdate(
kCompositingUpdateAfterGeometryChange);
}

// If we have fixed elements and we scroll the root layer we might
// change compositing since the fixed elements might now overlap a
// composited layer.
if (Layer()->IsRootLayer()) {
LocalFrame* frame = GetLayoutBox()->GetFrame();
if (frame && frame->View() &&
frame->View()->HasViewportConstrainedObjects()) {
Layer()->SetNeedsCompositingInputsUpdate();
}
}
} else {
Layer()->SetNeedsCompositingInputsUpdate();
}
}
