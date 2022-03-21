void GpuProcessHostUIShim::OnAcceleratedSurfaceNew(
const GpuHostMsg_AcceleratedSurfaceNew_Params& params) {
RenderWidgetHostViewPort* view = GetRenderWidgetHostViewFromSurfaceID(
params.surface_id);
if (!view)
return;
view->AcceleratedSurfaceNew(
      params.width, params.height, params.surface_handle);
}
