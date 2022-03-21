status_t OMXNodeInstance::useBuffer(
OMX_U32 portIndex, const sp<IMemory> &params,
OMX::buffer_id *buffer, OMX_U32 allottedSize) {
if (params == NULL || buffer == NULL) {
ALOGE("b/25884056");
return BAD_VALUE;

}

Mutex::Autolock autoLock(mLock);
    if (allottedSize > params->size()) {
return BAD_VALUE;
}

    BufferMeta *buffer_meta = new BufferMeta(params, portIndex);

OMX_BUFFERHEADERTYPE *header;

OMX_ERRORTYPE err = OMX_UseBuffer(
mHandle, &header, portIndex, buffer_meta,
            allottedSize, static_cast<OMX_U8 *>(params->pointer()));

if (err != OMX_ErrorNone) {
CLOG_ERROR(useBuffer, err, SIMPLE_BUFFER(
                portIndex, (size_t)allottedSize, params->pointer()));

delete buffer_meta;
buffer_meta = NULL;

*buffer = 0;

return StatusFromOMXError(err);
}

CHECK_EQ(header->pAppPrivate, buffer_meta);

*buffer = makeBufferID(header);

addActiveBuffer(portIndex, *buffer);

sp<GraphicBufferSource> bufferSource(getGraphicBufferSource());
if (bufferSource != NULL && portIndex == kPortIndexInput) {
bufferSource->addCodecBuffer(header);
}

CLOG_BUFFER(useBuffer, NEW_BUFFER_FMT(
*buffer, portIndex, "%u(%zu)@%p", allottedSize, params->size(), params->pointer()));
return OK;
}
