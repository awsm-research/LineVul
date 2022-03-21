void MediaControlsProgressView::HandleSeeking(
    const gfx::Point& location_in_bar) {
double seek_to_progress =
static_cast<double>(location_in_bar.x()) / progress_bar_->width();
seek_callback_.Run(seek_to_progress);
}
