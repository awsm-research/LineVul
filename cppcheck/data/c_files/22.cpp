void GLSurfaceOzoneSurfacelessSurfaceImpl::Destroy() {
if (!context_)
return;
scoped_refptr<gfx::GLContext> previous_context = gfx::GLContext::GetCurrent();
scoped_refptr<gfx::GLSurface> previous_surface;

bool was_current = previous_context && previous_context->IsCurrent(nullptr) &&
gfx::GLSurface::GetCurrent() == this;
if (!was_current) {
// Only take a reference to previous surface if it's not |this|
// because otherwise we can take a self reference from our own dtor.
previous_surface = gfx::GLSurface::GetCurrent();
context_->MakeCurrent(this);
}

glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
if (fbo_) {
glDeleteTextures(arraysize(textures_), textures_);
for (auto& texture : textures_)
texture = 0;
glDeleteFramebuffersEXT(1, &fbo_);
fbo_ = 0;
}
for (auto image : images_) {
if (image)
image->Destroy(true);
}

if (!was_current) {
    previous_context->MakeCurrent(previous_surface.get());
  } else {
    context_->ReleaseCurrent(this);
}
}
