void ShelfWidget::DelegateView::UpdateOpaqueBackground() {
const gfx::Rect local_bounds = GetLocalBounds();
gfx::Rect opaque_background_bounds = local_bounds;

const Shelf* shelf = shelf_widget_->shelf();
const ShelfBackgroundType background_type =
shelf_widget_->GetBackgroundType();

// If the app list is showing in clamshell mode, we should hide the shelf.
// otherwise, we should show it again. This creates a 'blending' effect
// between the two
if (background_type == SHELF_BACKGROUND_APP_LIST) {
opaque_background_.SetVisible(false);
UpdateBackgroundBlur();
return;
}

if (!opaque_background_.visible())
opaque_background_.SetVisible(true);

// Extend the opaque layer a little bit to handle "overshoot" gestures
// gracefully (the user drags the shelf further than it can actually go).
// That way:
// 1) When the shelf has rounded corners, only two of them are visible,
// 2) Even when the shelf is squared, it doesn't tear off the screen edge
// when dragged away.
// To achieve this, we extend the layer in the same direction where the shelf
// is aligned (downwards for a bottom shelf, etc.).
const int radius = kShelfRoundedCornerRadius;
const int safety_margin = 3 * radius;
opaque_background_bounds.Inset(
-shelf->SelectValueForShelfAlignment(0, safety_margin, 0), 0,
-shelf->SelectValueForShelfAlignment(0, 0, safety_margin),
-shelf->SelectValueForShelfAlignment(safety_margin, 0, 0));

  // Show rounded corners except in maximized and split modes.
  if (background_type == SHELF_BACKGROUND_MAXIMIZED ||
      background_type == SHELF_BACKGROUND_SPLIT_VIEW) {
mask_ = nullptr;
opaque_background_.SetMaskLayer(nullptr);
} else {
if (!mask_) {
mask_ = views::Painter::CreatePaintedLayer(
views::Painter::CreateSolidRoundRectPainter(SK_ColorBLACK, radius));
mask_->layer()->SetFillsBoundsOpaquely(false);
opaque_background_.SetMaskLayer(mask_->layer());
}
if (mask_->layer()->bounds() != opaque_background_bounds)
mask_->layer()->SetBounds(opaque_background_bounds);
}
opaque_background_.SetBounds(opaque_background_bounds);
UpdateBackgroundBlur();
SchedulePaint();
}
