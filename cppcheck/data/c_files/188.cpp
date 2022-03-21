void NavigationRequest::OnRequestRedirected(
const net::RedirectInfo& redirect_info,
const scoped_refptr<network::ResourceResponse>& response) {
response_ = response;
ssl_info_ = response->head.ssl_info;
#if defined(OS_ANDROID)
base::WeakPtr<NavigationRequest> this_ptr(weak_factory_.GetWeakPtr());

bool should_override_url_loading = false;
if (!GetContentClient()->browser()->ShouldOverrideUrlLoading(
frame_tree_node_->frame_tree_node_id(), browser_initiated_,
redirect_info.new_url, redirect_info.new_method,
// Redirects are always not counted as from user gesture.
false, true, frame_tree_node_->IsMainFrame(),
common_params_.transition, &should_override_url_loading)) {
// A Java exception was thrown by the embedding application; we
// need to return from this task. Specifically, it's not safe from
// this point on to make any JNI calls.
return;
}

// The content/ embedder might cause |this| to be deleted while
// |ShouldOverrideUrlLoading| is called.
// See https://crbug.com/770157.
if (!this_ptr)
return;

if (should_override_url_loading) {
navigation_handle_->set_net_error_code(net::ERR_ABORTED);
common_params_.url = redirect_info.new_url;
common_params_.method = redirect_info.new_method;
// Update the navigation handle to point to the new url to ensure
// AwWebContents sees the new URL and thus passes that URL to onPageFinished
// (rather than passing the old URL).
navigation_handle_->UpdateStateFollowingRedirect(
GURL(redirect_info.new_referrer),
base::Bind(&NavigationRequest::OnRedirectChecksComplete,
base::Unretained(this)));
frame_tree_node_->ResetNavigationRequest(false, true);
return;
}
#endif
if (!ChildProcessSecurityPolicyImpl::GetInstance()->CanRedirectToURL(
redirect_info.new_url)) {
DVLOG(1) << "Denied redirect for "
<< redirect_info.new_url.possibly_invalid_spec();
    navigation_handle_->set_net_error_code(net::ERR_UNSAFE_REDIRECT);
    frame_tree_node_->ResetNavigationRequest(false, true);
return;
}

// For renderer-initiated navigations we need to check if the source has
// access to the URL. Browser-initiated navigations only rely on the
// |CanRedirectToURL| test above.
if (!browser_initiated_ && source_site_instance() &&
!ChildProcessSecurityPolicyImpl::GetInstance()->CanRequestURL(
source_site_instance()->GetProcess()->GetID(),
redirect_info.new_url)) {
DVLOG(1) << "Denied unauthorized redirect for "
<< redirect_info.new_url.possibly_invalid_spec();
    navigation_handle_->set_net_error_code(net::ERR_UNSAFE_REDIRECT);
    frame_tree_node_->ResetNavigationRequest(false, true);
return;
}

// For now, DevTools needs the POST data sent to the renderer process even if
// it is no longer a POST after the redirect.
if (redirect_info.new_method != "POST")
common_params_.post_data = nullptr;

// Mark time for the Navigation Timing API.
if (commit_params_.navigation_timing.redirect_start.is_null()) {
commit_params_.navigation_timing.redirect_start =
commit_params_.navigation_timing.fetch_start;
}
commit_params_.navigation_timing.redirect_end = base::TimeTicks::Now();
commit_params_.navigation_timing.fetch_start = base::TimeTicks::Now();

commit_params_.redirect_response.push_back(response->head);
commit_params_.redirect_infos.push_back(redirect_info);

// On redirects, the initial origin_to_commit is no longer correct, so it
// must be cleared to avoid sending incorrect value to the renderer process.
if (commit_params_.origin_to_commit)
commit_params_.origin_to_commit.reset();

commit_params_.redirects.push_back(common_params_.url);
common_params_.url = redirect_info.new_url;
common_params_.method = redirect_info.new_method;
common_params_.referrer.url = GURL(redirect_info.new_referrer);
common_params_.referrer =
Referrer::SanitizeForRequest(common_params_.url, common_params_.referrer);

// Check Content Security Policy before the NavigationThrottles run. This
// gives CSP a chance to modify requests that NavigationThrottles would
// otherwise block.
net::Error net_error =
CheckContentSecurityPolicy(true /* has_followed_redirect */,
redirect_info.insecure_scheme_was_upgraded,
false /* is_response_check */);
if (net_error != net::OK) {
OnRequestFailedInternal(
network::URLLoaderCompletionStatus(net_error), false /*skip_throttles*/,
base::nullopt /*error_page_content*/, false /*collapse_frame*/);

// DO NOT ADD CODE after this. The previous call to OnRequestFailedInternal
// has destroyed the NavigationRequest.
return;
}

if (CheckCredentialedSubresource() ==
CredentialedSubresourceCheckResult::BLOCK_REQUEST ||
CheckLegacyProtocolInSubresource() ==
LegacyProtocolInSubresourceCheckResult::BLOCK_REQUEST) {
OnRequestFailedInternal(
network::URLLoaderCompletionStatus(net::ERR_ABORTED),
false /*skip_throttles*/, base::nullopt /*error_page_content*/,
false /*collapse_frame*/);

// DO NOT ADD CODE after this. The previous call to OnRequestFailedInternal
// has destroyed the NavigationRequest.
return;
}

// Compute the SiteInstance to use for the redirect and pass its
// RenderProcessHost if it has a process. Keep a reference if it has a
// process, so that the SiteInstance and its associated process aren't deleted
// before the navigation is ready to commit.
scoped_refptr<SiteInstance> site_instance =
frame_tree_node_->render_manager()->GetSiteInstanceForNavigationRequest(
*this);
speculative_site_instance_ =
site_instance->HasProcess() ? site_instance : nullptr;

// If the new site instance doesn't yet have a process, then tell the
// SpareRenderProcessHostManager so it can decide whether to start warming up
// the spare at this time (note that the actual behavior depends on
// RenderProcessHostImpl::IsSpareProcessKeptAtAllTimes).
if (!site_instance->HasProcess()) {
RenderProcessHostImpl::NotifySpareManagerAboutRecentlyUsedBrowserContext(
site_instance->GetBrowserContext());
}

// Re-evaluate the PreviewsState, but do not update the URLLoader. The
// URLLoader PreviewsState is considered immutable after the URLLoader is
// created.
common_params_.previews_state =
GetContentClient()->browser()->DetermineAllowedPreviews(
common_params_.previews_state, navigation_handle_.get(),
common_params_.url);

// Check what the process of the SiteInstance is. It will be passed to the
// NavigationHandle, and informed to expect a navigation to the redirected
// URL.
// Note: calling GetProcess on the SiteInstance can lead to the creation of a
// new process if it doesn't have one. In this case, it should only be called
// on a SiteInstance that already has a process.
RenderProcessHost* expected_process =
site_instance->HasProcess() ? site_instance->GetProcess() : nullptr;

// It's safe to use base::Unretained because this NavigationRequest owns the
// NavigationHandle where the callback will be stored.
navigation_handle_->WillRedirectRequest(
common_params_.referrer.url, expected_process,
base::Bind(&NavigationRequest::OnRedirectChecksComplete,
base::Unretained(this)));
}
