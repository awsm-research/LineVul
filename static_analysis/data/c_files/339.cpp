void Editor::ChangeSelectionAfterCommand(
const SelectionInDOMTree& new_selection,
const SetSelectionData& options) {
if (new_selection.IsNone())
return;

// See <rdar://problem/5729315> Some shouldChangeSelectedDOMRange contain
// Ranges for selections that are no longer valid
bool selection_did_not_change_dom_position =
new_selection == GetFrame().Selection().GetSelectionInDOMTree();
GetFrame().Selection().SetSelection(
      SelectionInDOMTree::Builder(new_selection)
          .SetIsHandleVisible(GetFrame().Selection().IsHandleVisible())
          .Build(),
      options);

// Some editing operations change the selection visually without affecting its
// position within the DOM. For example when you press return in the following
// (the caret is marked by ^):
// <div contentEditable="true"><div>^Hello</div></div>
// WebCore inserts <div><br></div> *before* the current block, which correctly
// moves the paragraph down but which doesn't change the caret's DOM position
// (["hello", 0]). In these situations the above FrameSelection::setSelection
// call does not call EditorClient::respondToChangedSelection(), which, on the
// Mac, sends selection change notifications and starts a new kill ring
// sequence, but we want to do these things (matches AppKit).
if (selection_did_not_change_dom_position) {
Client().RespondToChangedSelection(
frame_, GetFrame().Selection().GetSelectionInDOMTree().Type());
}
}
