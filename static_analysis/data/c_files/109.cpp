static void addDataToStreamTask(void* context)
{
OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    blobRegistry().addDataToStream(blobRegistryContext->url, blobRegistryContext->streamData);
}
