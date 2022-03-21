bool BaseAudioContext::WouldTaintOrigin(const KURL& url) const {
  // Data URLs don't taint the origin.
  if (url.ProtocolIsData()) {
    return false;
  }
  Document* document = GetDocument();
  if (document && document->GetSecurityOrigin()) {
    // The origin is tainted if and only if we cannot read content from the URL.
    return !document->GetSecurityOrigin()->CanRequest(url);
  }
  // Be conservative and assume it's tainted if it's not a data url and if we
  // can't get the security origin of the document.
  return true;
}
