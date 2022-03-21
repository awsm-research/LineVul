void AddSimpleRectChunk(TestPaintArtifact& artifact) {
    artifact
        .Chunk(TransformPaintPropertyNode::Root(),
               ClipPaintPropertyNode::Root(), EffectPaintPropertyNode::Root())
.RectDrawing(FloatRect(100, 100, 200, 100), Color::kBlack);
}
