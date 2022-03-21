void GpuVideoDecodeAccelerator::OnDecode(
    base::SharedMemoryHandle handle, int32 id, int32 size) {
DCHECK(video_decode_accelerator_.get());
video_decode_accelerator_->Decode(media::BitstreamBuffer(id, handle, size));
}
