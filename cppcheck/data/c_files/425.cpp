EffectPaintPropertyNode* EffectPaintPropertyNode::Root() {
  DEFINE_STATIC_REF(EffectPaintPropertyNode, root,
                    (EffectPaintPropertyNode::Create(
                        nullptr, State{TransformPaintPropertyNode::Root(),
                                       ClipPaintPropertyNode::Root()})));
return root;
}
