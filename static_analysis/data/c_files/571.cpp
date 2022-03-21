void ExtensionOptionsGuest::DidNavigateMainFrame(
const content::LoadCommittedDetails& details,
const content::FrameNavigateParams& params) {
if (attached()) {
auto guest_zoom_controller =
ui_zoom::ZoomController::FromWebContents(web_contents());
guest_zoom_controller->SetZoomMode(
ui_zoom::ZoomController::ZOOM_MODE_ISOLATED);
SetGuestZoomLevelToMatchEmbedder();

    if (params.url.GetOrigin() != options_page_.GetOrigin()) {
bad_message::ReceivedBadMessage(web_contents()->GetRenderProcessHost(),
bad_message::EOG_BAD_ORIGIN);
}
}
}
