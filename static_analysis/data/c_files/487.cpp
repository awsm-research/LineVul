i915_gem_execbuffer2(struct drm_device *dev, void *data,
struct drm_file *file)
{
struct drm_i915_gem_execbuffer2 *args = data;
struct drm_i915_gem_exec_object2 *exec2_list = NULL;
int ret;

	if (args->buffer_count < 1) {
DRM_DEBUG("execbuf2 with %d buffers\n", args->buffer_count);
return -EINVAL;
}

exec2_list = kmalloc(sizeof(*exec2_list)*args->buffer_count,
GFP_KERNEL | __GFP_NOWARN | __GFP_NORETRY);
if (exec2_list == NULL)
exec2_list = drm_malloc_ab(sizeof(*exec2_list),
args->buffer_count);
if (exec2_list == NULL) {
DRM_DEBUG("Failed to allocate exec list for %d buffers\n",
args->buffer_count);
return -ENOMEM;
}
ret = copy_from_user(exec2_list,
(struct drm_i915_relocation_entry __user *)
(uintptr_t) args->buffers_ptr,
sizeof(*exec2_list) * args->buffer_count);
if (ret != 0) {
DRM_DEBUG("copy %d exec entries failed %d\n",
args->buffer_count, ret);
drm_free_large(exec2_list);
return -EFAULT;
}

ret = i915_gem_do_execbuffer(dev, data, file, args, exec2_list);
if (!ret) {
/* Copy the new buffer offsets back to the user's exec list. */
ret = copy_to_user((struct drm_i915_relocation_entry __user *)
(uintptr_t) args->buffers_ptr,
exec2_list,
sizeof(*exec2_list) * args->buffer_count);
if (ret) {
ret = -EFAULT;
DRM_DEBUG("failed to copy %d exec entries "
"back to user (%d)\n",
args->buffer_count, ret);
}
}

drm_free_large(exec2_list);
return ret;
}
