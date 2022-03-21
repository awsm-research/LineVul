void VideoCaptureImpl::OnBufferCreated(int32_t buffer_id,
mojo::ScopedSharedBufferHandle handle) {
DVLOG(1) << __func__ << " buffer_id: " << buffer_id;
DCHECK(io_thread_checker_.CalledOnValidThread());
DCHECK(handle.is_valid());

base::SharedMemoryHandle memory_handle;
size_t memory_size = 0;
  bool read_only_flag = false;

const MojoResult result = mojo::UnwrapSharedMemoryHandle(
      std::move(handle), &memory_handle, &memory_size, &read_only_flag);
DCHECK_EQ(MOJO_RESULT_OK, result);
DCHECK_GT(memory_size, 0u);

std::unique_ptr<base::SharedMemory> shm(
new base::SharedMemory(memory_handle, true /* read_only */));
if (!shm->Map(memory_size)) {
DLOG(ERROR) << "OnBufferCreated: Map failed.";
return;
}
const bool inserted =
client_buffers_
.insert(std::make_pair(buffer_id,
new ClientBuffer(std::move(shm), memory_size)))
.second;
DCHECK(inserted);
}
