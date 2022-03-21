void DownloadResourceHandler::OnRequestRedirected(
const net::RedirectInfo& redirect_info,
network::ResourceResponse* response,
std::unique_ptr<ResourceController> controller) {
url::Origin new_origin(url::Origin::Create(redirect_info.new_url));
if (!follow_cross_origin_redirects_ &&
!first_origin_.IsSameOriginWith(new_origin)) {
base::PostTaskWithTraits(
FROM_HERE, {BrowserThread::UI},
base::BindOnce(
&NavigateOnUIThread, redirect_info.new_url, request()->url_chain(),
Referrer(GURL(redirect_info.new_referrer),
Referrer::NetReferrerPolicyToBlinkReferrerPolicy(
redirect_info.new_referrer_policy)),
GetRequestInfo()->HasUserGesture(),
            GetRequestInfo()->GetWebContentsGetterForRequest()));
controller->Cancel();
return;
}
if (core_.OnRequestRedirected()) {
controller->Resume();
} else {
controller->Cancel();
}
}
