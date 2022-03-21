void FrameSelection::MoveRangeSelection(const VisiblePosition& base_position,
const VisiblePosition& extent_position,
TextGranularity granularity) {
SelectionInDOMTree new_selection =
SelectionInDOMTree::Builder()
.SetBaseAndExtentDeprecated(base_position.DeepEquivalent(),
extent_position.DeepEquivalent())
.SetAffinity(base_position.Affinity())
          .SetIsHandleVisible(IsHandleVisible())
.Build();

if (new_selection.IsNone())
return;

const VisibleSelection& visible_selection =
CreateVisibleSelectionWithGranularity(new_selection, granularity);
if (visible_selection.IsNone())
return;

SelectionInDOMTree::Builder builder;
if (visible_selection.IsBaseFirst()) {
builder.SetBaseAndExtent(visible_selection.Start(),
visible_selection.End());
} else {
builder.SetBaseAndExtent(visible_selection.End(),
visible_selection.Start());
}
builder.SetAffinity(visible_selection.Affinity());
  builder.SetIsHandleVisible(IsHandleVisible());
SetSelection(builder.Build(), SetSelectionData::Builder()
.SetShouldCloseTyping(true)
.SetShouldClearTypingStyle(true)
.SetGranularity(granularity)
.Build());
}
