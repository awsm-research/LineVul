void FragmentPaintPropertyTreeBuilder::UpdateFragmentClip() {
DCHECK(properties_);

if (NeedsPaintPropertyUpdate()) {
if (context_.fragment_clip) {
OnUpdateClip(properties_->UpdateFragmentClip(
          context_.current.clip,
ClipPaintPropertyNode::State{context_.current.transform,
ToClipRect(*context_.fragment_clip)}));
} else {
OnClearClip(properties_->ClearFragmentClip());
}
}

if (properties_->FragmentClip())
context_.current.clip = properties_->FragmentClip();
}
