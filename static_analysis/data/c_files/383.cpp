void SkiaOutputSurfaceImpl::Reshape(const gfx::Size& size,
float device_scale_factor,
const gfx::ColorSpace& color_space,
bool has_alpha,
bool use_stencil) {
DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
if (initialize_waitable_event_) {
initialize_waitable_event_->Wait();
    initialize_waitable_event_ = nullptr;
}

SkSurfaceCharacterization* characterization = nullptr;
if (characterization_.isValid()) {
    // TODO(weiliang): support color space. https://crbug.com/795132
    characterization_ =
        characterization_.createResized(size.width(), size.height());
RecreateRootRecorder();
} else {
characterization = &characterization_;
initialize_waitable_event_ = std::make_unique<base::WaitableEvent>(
base::WaitableEvent::ResetPolicy::MANUAL,
base::WaitableEvent::InitialState::NOT_SIGNALED);
}

// impl_on_gpu_ is released on the GPU thread by a posted task from
// SkiaOutputSurfaceImpl::dtor. So it is safe to use base::Unretained.
  auto callback = base::BindOnce(
      &SkiaOutputSurfaceImplOnGpu::Reshape,
      base::Unretained(impl_on_gpu_.get()), size, device_scale_factor,
      std::move(color_space), has_alpha, use_stencil, pre_transform_,
      characterization, initialize_waitable_event_.get());
  ScheduleGpuTask(std::move(callback), std::vector<gpu::SyncToken>());
}
