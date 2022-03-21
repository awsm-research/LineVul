BaseRenderingContext2D::BaseRenderingContext2D()
    : clip_antialiasing_(kNotAntiAliased) {
state_stack_.push_back(CanvasRenderingContext2DState::Create());
}
