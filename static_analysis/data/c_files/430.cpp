status_t OMXNodeInstance::allocateBufferWithBackup(
OMX_U32 portIndex, const sp<IMemory> &params,
OMX::buffer_id *buffer, OMX_U32 allottedSize) {
Mutex::Autolock autoLock(mLock);
if (allottedSize > params->size()) {

return BAD_VALUE;
}

    BufferMeta *buffer_meta = new BufferMeta(params, true);

OMX_BUFFERHEADERTYPE *header;

OMX_ERRORTYPE err = OMX_AllocateBuffer(
mHandle, &header, portIndex, buffer_meta, allottedSize);
if (err != OMX_ErrorNone) {
CLOG_ERROR(allocateBufferWithBackup, err,
SIMPLE_BUFFER(portIndex, (size_t)allottedSize, params->pointer()));
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

CLOG_BUFFER(allocateBufferWithBackup, NEW_BUFFER_FMT(*buffer, portIndex, "%zu@%p :> %u@%p",
params->size(), params->pointer(), allottedSize, header->pBuffer));

return OK;
}
