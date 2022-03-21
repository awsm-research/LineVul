void OffscreenCanvasFrameReceiverImpl::SubmitCompositorFrame(
const cc::SurfaceId& surface_id,
cc::CompositorFrame frame) {
    cc::Surface* surface = GetSurfaceManager()->GetSurfaceForId(surface_id);
    if (surface) {
        surface->QueueFrame(std::move(frame), base::Closure());
}
    // If surface doet not exist, drop the frame.
}
