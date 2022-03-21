void ThreadableBlobRegistry::registerBlobURL(SecurityOrigin* origin, const KURL& url, const KURL& srcURL)
{
// If the blob URL contains null origin, as in the context with unique
// security origin or file URL, save the mapping between url and origin so
// that the origin can be retrived when doing security origin check.
if (origin && BlobURL::getOrigin(url) == "null")
originMap()->add(url.string(), origin);

    if (isMainThread())
        blobRegistry().registerBlobURL(url, srcURL);
    else {
OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url, srcURL));
callOnMainThread(&registerBlobURLFromTask, context.leakPtr());
}
}
