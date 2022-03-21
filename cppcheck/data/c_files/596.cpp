void PartialMagnificationController::OnWidgetDestroying(views::Widget* widget) {
  DCHECK_EQ(widget, zoom_widget_);
RemoveZoomWidgetObservers();
  zoom_widget_ = NULL;
}
