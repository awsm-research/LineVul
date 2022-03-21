static jboolean enableNative(JNIEnv* env, jobject obj) {
ALOGV("%s:",__FUNCTION__);

jboolean result = JNI_FALSE;
if (!sBluetoothInterface) return result;
    int ret = sBluetoothInterface->enable();
result = (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_DONE) ? JNI_TRUE : JNI_FALSE;
return result;
}
