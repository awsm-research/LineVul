Blob::Blob(PassOwnPtr<BlobData> blobData, long long size)
: m_type(blobData->contentType())
, m_size(size)
{
ASSERT(blobData);
ScriptWrappable::init(this);

// Create a new internal URL and register it with the provided blob data.
m_internalURL = BlobURL::createInternalURL();
    ThreadableBlobRegistry::registerBlobURL(m_internalURL, blobData);
}
