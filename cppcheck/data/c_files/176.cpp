void MojoJpegDecodeAccelerator::Decode(
const BitstreamBuffer& bitstream_buffer,
const scoped_refptr<VideoFrame>& video_frame) {
DCHECK(io_task_runner_->BelongsToCurrentThread());
DCHECK(jpeg_decoder_.is_bound());

DCHECK(
base::SharedMemory::IsHandleValid(video_frame->shared_memory_handle()));

base::SharedMemoryHandle output_handle =
base::SharedMemory::DuplicateHandle(video_frame->shared_memory_handle());
if (!base::SharedMemory::IsHandleValid(output_handle)) {
DLOG(ERROR) << "Failed to duplicate handle of VideoFrame";
return;
}

size_t output_buffer_size = VideoFrame::AllocationSize(
video_frame->format(), video_frame->coded_size());
mojo::ScopedSharedBufferHandle output_frame_handle =
      mojo::WrapSharedMemoryHandle(output_handle, output_buffer_size,
                                   false /* read_only */);

// base::Unretained is safe because |this| owns |jpeg_decoder_|.
jpeg_decoder_->Decode(bitstream_buffer, video_frame->coded_size(),
std::move(output_frame_handle),
base::checked_cast<uint32_t>(output_buffer_size),
base::Bind(&MojoJpegDecodeAccelerator::OnDecodeAck,
base::Unretained(this)));
}
