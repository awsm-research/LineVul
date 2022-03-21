static SharedMemorySupport DoQuerySharedMemorySupport(Display* dpy) {
int dummy;
Bool pixmaps_supported;
// Query the server's support for XSHM.
if (!XShmQueryVersion(dpy, &dummy, &dummy, &pixmaps_supported))
return SHARED_MEMORY_NONE;

#if defined(OS_FREEBSD)
// On FreeBSD we can't access the shared memory after it was marked for
// deletion, unless this behaviour is explicitly enabled by the user.
// In case it's not enabled disable shared memory support.
int allow_removed;
size_t length = sizeof(allow_removed);

if ((sysctlbyname("kern.ipc.shm_allow_removed", &allow_removed, &length,
NULL, 0) < 0) || allow_removed < 1) {
return SHARED_MEMORY_NONE;
}
#endif

// Next we probe to see if shared memory will really work
  int shmkey = shmget(IPC_PRIVATE, 1, 0666);
  if (shmkey == -1)
return SHARED_MEMORY_NONE;
void* address = shmat(shmkey, NULL, 0);
// Mark the shared memory region for deletion
shmctl(shmkey, IPC_RMID, NULL);

XShmSegmentInfo shminfo;
memset(&shminfo, 0, sizeof(shminfo));
shminfo.shmid = shmkey;

gdk_error_trap_push();
bool result = XShmAttach(dpy, &shminfo);
XSync(dpy, False);
if (gdk_error_trap_pop())
result = false;
shmdt(address);
  if (!result)
return SHARED_MEMORY_NONE;

XShmDetach(dpy, &shminfo);
return pixmaps_supported ? SHARED_MEMORY_PIXMAP : SHARED_MEMORY_PUTIMAGE;
}
