void RenderWidgetHostImpl::DidNavigate(uint32_t next_source_id) {
current_content_source_id_ = next_source_id;
did_receive_first_frame_after_navigation_ = false;

if (enable_surface_synchronization_) {
// Resize messages before navigation are not acked, so reset
// |visual_properties_ack_pending_| and make sure the next resize will be
// acked if the last resize before navigation was supposed to be acked.
visual_properties_ack_pending_ = false;
    viz::LocalSurfaceId old_surface_id = view_->GetLocalSurfaceId();
if (view_)
view_->DidNavigate();
    viz::LocalSurfaceId new_surface_id = view_->GetLocalSurfaceId();
    // If |view_| didn't allocate a new surface id, then don't start
    // |new_content_rendering_timeout_|. Two reasons:
    //  1. It's not needed (because this was the first navigation event)
    //  2. If we don't change the surface id, then we will not get the call to
    //     OnFirstSurfaceActivation, and not stop the timer (even if we get new
    //     frames).
    // https://crbug.com/853651, https://crbug.com/535375
    if (old_surface_id == new_surface_id)
      return;
} else {
// It is possible for a compositor frame to arrive before the browser is
// notified about the page being committed, in which case no timer is
// necessary.
if (last_received_content_source_id_ >= current_content_source_id_)
return;
}

if (!new_content_rendering_timeout_)
return;

new_content_rendering_timeout_->Start(new_content_rendering_delay_);
}
