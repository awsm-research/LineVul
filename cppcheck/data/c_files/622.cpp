const CuePoint* Cues::GetLast() const {
  if (m_cue_points == NULL)
return NULL;

  if (m_count <= 0)
    return NULL;
#if 0
    LoadCuePoint();  //init cues
    const size_t count = m_count + m_preload_count;
    if (count == 0)  //weird
        return NULL;
    const size_t index = count - 1;
    CuePoint* const* const pp = m_cue_points;
    assert(pp);
    CuePoint* const pCP = pp[index];
    assert(pCP);
    pCP->Load(m_pSegment->m_pReader);
    assert(pCP->GetTimeCode() >= 0);
#else
const long index = m_count - 1;

CuePoint* const* const pp = m_cue_points;
  assert(pp);

CuePoint* const pCP = pp[index];
  assert(pCP);
  assert(pCP->GetTimeCode() >= 0);
#endif

return pCP;
}
