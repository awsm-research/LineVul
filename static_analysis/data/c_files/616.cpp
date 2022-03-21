long long Segment::ParseHeaders() {
// Outermost (level 0) segment object has been constructed,
// and pos designates start of payload.  We need to find the
// inner (level 1) elements.
long long total, available;

const int status = m_pReader->Length(&total, &available);


if (status < 0)  // error
return status;

  assert((total < 0) || (available <= total));

const long long segment_stop = (m_size < 0) ? -1 : m_start + m_size;
  assert((segment_stop < 0) || (total < 0) || (segment_stop <= total));
  assert((segment_stop < 0) || (m_pos <= segment_stop));

for (;;) {
if ((total >= 0) && (m_pos >= total))
break;

if ((segment_stop >= 0) && (m_pos >= segment_stop))
break;


long long pos = m_pos;
const long long element_start = pos;

if ((pos + 1) > available)
return (pos + 1);

long len;
long long result = GetUIntLength(m_pReader, pos, len);


if (result < 0)  // error
return result;

    if (result > 0)  // underflow (weird)
return (pos + 1);

if ((segment_stop >= 0) && ((pos + len) > segment_stop))
return E_FILE_FORMAT_INVALID;

if ((pos + len) > available)

return pos + len;

const long long idpos = pos;
    const long long id = ReadUInt(m_pReader, idpos, len);

    if (id < 0)  // error
      return id;

if (id == 0x0F43B675)  // Cluster ID
break;

pos += len; // consume ID

if ((pos + 1) > available)
return (pos + 1);

// Read Size
result = GetUIntLength(m_pReader, pos, len);


if (result < 0)  // error
return result;

    if (result > 0)  // underflow (weird)
return (pos + 1);

if ((segment_stop >= 0) && ((pos + len) > segment_stop))
return E_FILE_FORMAT_INVALID;

if ((pos + len) > available)
return pos + len;


const long long size = ReadUInt(m_pReader, pos, len);

    if (size < 0)  // error
return size;

pos += len;  // consume length of size of element

const long long element_size = size + pos - element_start;

// Pos now points to start of payload

if ((segment_stop >= 0) && ((pos + size) > segment_stop))
return E_FILE_FORMAT_INVALID;

// We read EBML elements either in total or nothing at all.

if ((pos + size) > available)
return pos + size;

if (id == 0x0549A966) { // Segment Info ID
if (m_pInfo)
return E_FILE_FORMAT_INVALID;

m_pInfo = new (std::nothrow)
SegmentInfo(this, pos, size, element_start, element_size);

if (m_pInfo == NULL)
return -1;

const long status = m_pInfo->Parse();

if (status)
return status;
} else if (id == 0x0654AE6B) { // Tracks ID
if (m_pTracks)
return E_FILE_FORMAT_INVALID;

m_pTracks = new (std::nothrow)
Tracks(this, pos, size, element_start, element_size);

if (m_pTracks == NULL)
return -1;

const long status = m_pTracks->Parse();

if (status)
return status;
} else if (id == 0x0C53BB6B) { // Cues ID
if (m_pCues == NULL) {
m_pCues = new (std::nothrow)
Cues(this, pos, size, element_start, element_size);

if (m_pCues == NULL)
return -1;
}
} else if (id == 0x014D9B74) { // SeekHead ID
if (m_pSeekHead == NULL) {
m_pSeekHead = new (std::nothrow)
SeekHead(this, pos, size, element_start, element_size);

if (m_pSeekHead == NULL)
return -1;

const long status = m_pSeekHead->Parse();

if (status)
return status;
}
} else if (id == 0x0043A770) { // Chapters ID
if (m_pChapters == NULL) {
m_pChapters = new (std::nothrow)
Chapters(this, pos, size, element_start, element_size);

if (m_pChapters == NULL)
return -1;

const long status = m_pChapters->Parse();


if (status)
return status;
}
}

m_pos = pos + size;  // consume payload
}

  assert((segment_stop < 0) || (m_pos <= segment_stop));

if (m_pInfo == NULL)  // TODO: liberalize this behavior
return E_FILE_FORMAT_INVALID;

if (m_pTracks == NULL)
return E_FILE_FORMAT_INVALID;

return 0; // success
}
