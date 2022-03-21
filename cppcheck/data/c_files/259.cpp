void ExtensionViewGuest::DidNavigateMainFrame(
const content::LoadCommittedDetails& details,
const content::FrameNavigateParams& params) {
  if (attached() && (params.url.GetOrigin() != url_.GetOrigin())) {
bad_message::ReceivedBadMessage(web_contents()->GetRenderProcessHost(),
bad_message::EVG_BAD_ORIGIN);
}
}
