DrawingBuffer::DrawingBuffer(
std::unique_ptr<WebGraphicsContext3DProvider> context_provider,
std::unique_ptr<Extensions3DUtil> extensions_util,
Client* client,
bool discard_framebuffer_supported,
bool want_alpha_channel,
bool premultiplied_alpha,
PreserveDrawingBuffer preserve,
    WebGLVersion web_gl_version,
bool want_depth,
bool want_stencil,
ChromiumImageUsage chromium_image_usage,
const CanvasColorParams& color_params)
: client_(client),
preserve_drawing_buffer_(preserve),
      web_gl_version_(web_gl_version),
context_provider_(WTF::WrapUnique(new WebGraphicsContext3DProviderWrapper(
std::move(context_provider)))),
gl_(this->ContextProvider()->ContextGL()),
extensions_util_(std::move(extensions_util)),
discard_framebuffer_supported_(discard_framebuffer_supported),
want_alpha_channel_(want_alpha_channel),
premultiplied_alpha_(premultiplied_alpha),
software_rendering_(this->ContextProvider()->IsSoftwareRendering()),
want_depth_(want_depth),
want_stencil_(want_stencil),
color_space_(color_params.GetGfxColorSpace()),
chromium_image_usage_(chromium_image_usage) {
// Used by browser tests to detect the use of a DrawingBuffer.
TRACE_EVENT_INSTANT0("test_gpu", "DrawingBufferCreation",
TRACE_EVENT_SCOPE_GLOBAL);
}
