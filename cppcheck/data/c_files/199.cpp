bool ChildProcessSecurityPolicyImpl::CanSetAsOriginHeader(int child_id,
const GURL& url) {
if (!url.is_valid())
return false;  // Can't set invalid URLs as origin headers.

// about:srcdoc cannot be used as an origin
if (url == kAboutSrcDocURL)
return false;

// If this process can commit |url|, it can use |url| as an origin for
// outbound requests.
  if (CanCommitURL(child_id, url))
return true;

// Allow schemes which may come from scripts executing in isolated worlds;
// XHRs issued by such scripts reflect the script origin rather than the
// document origin.
{
base::AutoLock lock(lock_);
if (base::ContainsKey(schemes_okay_to_appear_as_origin_headers_,
url.scheme())) {
return true;
}
}
return false;
}
