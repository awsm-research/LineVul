RenderWidgetHostViewAndroid::RenderWidgetHostViewAndroid(
RenderWidgetHostImpl* widget_host,
ContentViewCoreImpl* content_view_core)
: host_(widget_host),
is_layer_attached_(true),
content_view_core_(NULL),
ime_adapter_android_(ALLOW_THIS_IN_INITIALIZER_LIST(this)),
cached_background_color_(SK_ColorWHITE),
      texture_id_in_layer_(0) {
if (CompositorImpl::UsesDirectGL()) {
surface_texture_transport_.reset(new SurfaceTextureTransportClient());
layer_ = surface_texture_transport_->Initialize();
} else {
texture_layer_ = cc::TextureLayer::create(0);
layer_ = texture_layer_;
}

layer_->setContentsOpaque(true);
layer_->setIsDrawable(true);

host_->SetView(this);
SetContentViewCore(content_view_core);
}
