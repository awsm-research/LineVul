void NavigationController::GoToIndex(int index) {
if (index < 0 || index >= static_cast<int>(entries_.size())) {
NOTREACHED();
return;
}

if (transient_entry_index_ != -1) {
if (index == transient_entry_index_) {
// Nothing to do when navigating to the transient.
return;
}
if (index > transient_entry_index_) {
// Removing the transient is goint to shift all entries by 1.
index--;
}
}

  // If an interstitial page is showing, the previous renderer is blocked and
  // cannot make new requests.
  if (tab_contents_->interstitial_page()) {
    if (index == GetCurrentEntryIndex() - 1) {
      // Going back one entry is equivalent to hiding the interstitial.
      tab_contents_->interstitial_page()->DontProceed();
      return;
    } else {
      // Unblock the renderer (and disable the interstitial) to allow this
      // navigation to succeed.  The interstitial will stay visible until the
      // resulting DidNavigate.
      tab_contents_->interstitial_page()->CancelForNavigation();
    }
  }
DiscardNonCommittedEntries();

pending_entry_index_ = index;
entries_[pending_entry_index_]->set_transition_type(
content::PageTransitionFromInt(
entries_[pending_entry_index_]->transition_type() |
content::PAGE_TRANSITION_FORWARD_BACK));
NavigateToPendingEntry(NO_RELOAD);
}
