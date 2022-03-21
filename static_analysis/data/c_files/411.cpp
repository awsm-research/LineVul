Document* LocalDOMWindow::InstallNewDocument(const String& mime_type,
const DocumentInit& init,
bool force_xhtml) {
DCHECK_EQ(init.GetFrame(), GetFrame());

ClearDocument();

document_ = CreateDocument(mime_type, init, force_xhtml);
event_queue_ = DOMWindowEventQueue::Create(document_.Get());
document_->Initialize();

if (!GetFrame())
return document_;

GetFrame()->GetScriptController().UpdateDocument();
document_->UpdateViewportDescription();

if (GetFrame()->GetPage() && GetFrame()->View()) {
GetFrame()->GetPage()->GetChromeClient().InstallSupplements(*GetFrame());

if (ScrollingCoordinator* scrolling_coordinator =
GetFrame()->GetPage()->GetScrollingCoordinator()) {
scrolling_coordinator->ScrollableAreaScrollbarLayerDidChange(
GetFrame()->View(), kHorizontalScrollbar);
scrolling_coordinator->ScrollableAreaScrollbarLayerDidChange(
GetFrame()->View(), kVerticalScrollbar);
scrolling_coordinator->ScrollableAreaScrollLayerDidChange(
GetFrame()->View());
}
}

  GetFrame()->Selection().UpdateSecureKeyboardEntryIfActive();
if (GetFrame()->IsCrossOriginSubframe())
document_->RecordDeferredLoadReason(WouldLoadReason::kCreated);

return document_;
}
