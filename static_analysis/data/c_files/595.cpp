status_t SoundTriggerHwService::Module::startRecognition(sound_model_handle_t handle,
const sp<IMemory>& dataMemory)
{
ALOGV("startRecognition() model handle %d", handle);
if (!captureHotwordAllowed()) {

return PERMISSION_DENIED;
}

    if (dataMemory != 0 && dataMemory->pointer() == NULL) {
        ALOGE("startRecognition() dataMemory is non-0 but has NULL pointer()");
return BAD_VALUE;

}
AutoMutex lock(mLock);
if (mServiceState == SOUND_TRIGGER_STATE_DISABLED) {
return INVALID_OPERATION;
}
sp<Model> model = getModel(handle);

if (model == 0) {
return BAD_VALUE;
}
    if ((dataMemory == 0) ||
            (dataMemory->size() < sizeof(struct sound_trigger_recognition_config))) {
        return BAD_VALUE;
    }

if (model->mState == Model::STATE_ACTIVE) {
return INVALID_OPERATION;
}

    struct sound_trigger_recognition_config *config =
            (struct sound_trigger_recognition_config *)dataMemory->pointer();

//TODO: get capture handle and device from audio policy service
config->capture_handle = model->mCaptureIOHandle;
config->capture_device = model->mCaptureDevice;
status_t status = mHwDevice->start_recognition(mHwDevice, handle, config,
SoundTriggerHwService::recognitionCallback,
this);

if (status == NO_ERROR) {
model->mState = Model::STATE_ACTIVE;
model->mConfig = *config;
}

return status;
}
