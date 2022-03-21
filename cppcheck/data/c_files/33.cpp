StateBase* writeBlob(v8::Handle<v8::Value> value, StateBase* next)
{
Blob* blob = V8Blob::toNative(value.As<v8::Object>());
if (!blob)
return 0;
if (blob->hasBeenClosed())
return handleError(DataCloneError, "A Blob object has been closed, and could therefore not be cloned.", next);
int blobIndex = -1;
        m_blobDataHandles.add(blob->uuid(), blob->blobDataHandle());
if (appendBlobInfo(blob->uuid(), blob->type(), blob->size(), &blobIndex))
m_writer.writeBlobIndex(blobIndex);
else
m_writer.writeBlob(blob->uuid(), blob->type(), blob->size());
return 0;
}
