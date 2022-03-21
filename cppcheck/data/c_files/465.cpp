void FragmentPaintPropertyTreeBuilder::UpdateClipPathClip(
bool spv1_compositing_specific_pass) {
// In SPv1*, composited path-based clip-path applies to a mask paint chunk
// instead of actual contents. We have to delay until mask clip node has been
// created first so we can parent under it.
bool is_spv1_composited =
object_.HasLayer() &&
ToLayoutBoxModelObject(object_).Layer()->GetCompositedLayerMapping();
if (is_spv1_composited != spv1_compositing_specific_pass)
return;

if (NeedsPaintPropertyUpdate()) {
if (!NeedsClipPathClip(object_)) {
OnClearClip(properties_->ClearClipPathClip());
} else {
ClipPaintPropertyNode::State state;
state.local_transform_space = context_.current.transform;
state.clip_rect =
FloatRoundedRect(FloatRect(*fragment_data_.ClipPathBoundingBox()));
state.clip_path = fragment_data_.ClipPathPath();
      OnUpdateClip(properties_->UpdateClipPathClip(context_.current.clip,
std::move(state)));
}
}

if (properties_->ClipPathClip() && !spv1_compositing_specific_pass) {
context_.current.clip = context_.absolute_position.clip =
context_.fixed_position.clip = properties_->ClipPathClip();
}
}
