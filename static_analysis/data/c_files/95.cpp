void GpuVideoDecodeAccelerator::Initialize(
const media::VideoCodecProfile profile,
    IPC::Message* init_done_msg,
    base::ProcessHandle renderer_process) {
DCHECK(!video_decode_accelerator_.get());
DCHECK(!init_done_msg_);
DCHECK(init_done_msg);
init_done_msg_ = init_done_msg;

#if (defined(OS_CHROMEOS) && defined(ARCH_CPU_ARMEL)) || defined(OS_WIN)
DCHECK(stub_ && stub_->decoder());
#if defined(OS_WIN)
if (base::win::GetVersion() < base::win::VERSION_WIN7) {
NOTIMPLEMENTED() << "HW video decode acceleration not available.";
NotifyError(media::VideoDecodeAccelerator::PLATFORM_FAILURE);
return;
}
DLOG(INFO) << "Initializing DXVA HW decoder for windows.";
DXVAVideoDecodeAccelerator* video_decoder =
      new DXVAVideoDecodeAccelerator(this, renderer_process);
#else  // OS_WIN
OmxVideoDecodeAccelerator* video_decoder =
new OmxVideoDecodeAccelerator(this);
video_decoder->SetEglState(
gfx::GLSurfaceEGL::GetHardwareDisplay(),
stub_->decoder()->GetGLContext()->GetHandle());
#endif  // OS_WIN
video_decode_accelerator_ = video_decoder;
if (!video_decode_accelerator_->Initialize(profile))
NotifyError(media::VideoDecodeAccelerator::PLATFORM_FAILURE);
#else  // Update RenderViewImpl::createMediaPlayer when adding clauses.
NOTIMPLEMENTED() << "HW video decode acceleration not available.";
NotifyError(media::VideoDecodeAccelerator::PLATFORM_FAILURE);
#endif  // defined(OS_CHROMEOS) && defined(ARCH_CPU_ARMEL)
}
