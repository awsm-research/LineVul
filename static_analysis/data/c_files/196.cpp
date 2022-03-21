void TextTrack::addCue(TextTrackCue* cue) {
DCHECK(cue);

  // TODO(93143): Add spec-compliant behavior for negative time values.
  if (std::isnan(cue->startTime()) || std::isnan(cue->endTime()) ||
      cue->startTime() < 0 || cue->endTime() < 0)
return;

// https://html.spec.whatwg.org/multipage/embedded-content.html#dom-texttrack-addcue

// The addCue(cue) method of TextTrack objects, when invoked, must run the
// following steps:

// (Steps 1 and 2 - pertaining to association of rendering rules - are not
// implemented.)

// 3. If the given cue is in a text track list of cues, then remove cue
// from that text track list of cues.
if (TextTrack* cue_track = cue->track())
cue_track->removeCue(cue, ASSERT_NO_EXCEPTION);

// 4. Add cue to the method's TextTrack object's text track's text track list
// of cues.
cue->SetTrack(this);
EnsureTextTrackCueList()->Add(cue);

if (GetCueTimeline() && mode_ != DisabledKeyword())
GetCueTimeline()->AddCue(this, cue);
}
