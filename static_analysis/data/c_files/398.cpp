bool SiteInstanceImpl::ShouldLockToOrigin(BrowserContext* browser_context,
GURL site_url) {
// Don't lock to origin in --single-process mode, since this mode puts
// cross-site pages into the same process.
if (RenderProcessHost::run_renderer_in_process())
return false;

if (!DoesSiteRequireDedicatedProcess(browser_context, site_url))
return false;

// Guest processes cannot be locked to their site because guests always have
// a fixed SiteInstance. The site of GURLs a guest loads doesn't match that
// SiteInstance. So we skip locking the guest process to the site.
// TODO(ncarter): Remove this exclusion once we can make origin lock per
// RenderFrame routing id.
if (site_url.SchemeIs(content::kGuestScheme))
return false;

  // TODO(creis, nick) https://crbug.com/510588 Chrome UI pages use the same
  // site (chrome://chrome), so they can't be locked because the site being
  // loaded doesn't match the SiteInstance.
  if (site_url.SchemeIs(content::kChromeUIScheme))
    return false;
// TODO(creis, nick): Until we can handle sites with effective URLs at the
// call sites of ChildProcessSecurityPolicy::CanAccessDataForOrigin, we
// must give the embedder a chance to exempt some sites to avoid process
// kills.
if (!GetContentClient()->browser()->ShouldLockToOrigin(browser_context,
site_url)) {
return false;
}

return true;
}
