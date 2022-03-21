bool GpuProcessPolicy::PreSandboxHook() {
// Warm up resources needed by the policy we're about to enable and
// eventually start a broker process.
const bool chromeos_arm_gpu = IsChromeOS() && IsArchitectureArm();
// This policy is for x86 or Desktop.
DCHECK(!chromeos_arm_gpu);

DCHECK(!broker_process());
// Create a new broker process.
InitGpuBrokerProcess(
GpuBrokerProcessPolicy::Create,
std::vector<BrokerFilePermission>());  // No extra files in whitelist.

if (IsArchitectureX86_64() || IsArchitectureI386()) {
// Accelerated video dlopen()'s some shared objects
// inside the sandbox, so preload them now.
if (IsAcceleratedVaapiVideoEncodeEnabled() ||
IsAcceleratedVideoDecodeEnabled()) {
const char* I965DrvVideoPath = NULL;
      const char* I965HybridDrvVideoPath = NULL;

if (IsArchitectureX86_64()) {
I965DrvVideoPath = "/usr/lib64/va/drivers/i965_drv_video.so";
        I965HybridDrvVideoPath = "/usr/lib64/va/drivers/hybrid_drv_video.so";
} else if (IsArchitectureI386()) {
I965DrvVideoPath = "/usr/lib/va/drivers/i965_drv_video.so";
}

dlopen(I965DrvVideoPath, RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
      if (I965HybridDrvVideoPath)
        dlopen(I965HybridDrvVideoPath, RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
dlopen("libva.so.1", RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
#if defined(USE_OZONE)
dlopen("libva-drm.so.1", RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
#elif defined(USE_X11)
dlopen("libva-x11.so.1", RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
#endif
}
}

return true;
}
