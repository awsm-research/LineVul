long Segment::ParseCues(long long off, long long& pos, long& len) {
if (m_pCues)
return 0;  // success

if (off < 0)
return -1;

long long total, avail;

const int status = m_pReader->Length(&total, &avail);

if (status < 0) // error
return status;

assert((total < 0) || (avail <= total));

pos = m_start + off;

if ((total < 0) || (pos >= total))
return 1; // don't bother parsing cues

const long long element_start = pos;
const long long segment_stop = (m_size < 0) ? -1 : m_start + m_size;

if ((pos + 1) > avail) {
len = 1;
return E_BUFFER_NOT_FULL;
}

long long result = GetUIntLength(m_pReader, pos, len);

if (result < 0) // error
return static_cast<long>(result);

if (result > 0) // underflow (weird)
{
len = 1;
return E_BUFFER_NOT_FULL;
}

if ((segment_stop >= 0) && ((pos + len) > segment_stop))
return E_FILE_FORMAT_INVALID;

if ((pos + len) > avail)
return E_BUFFER_NOT_FULL;


const long long idpos = pos;

  const long long id = ReadUInt(m_pReader, idpos, len);

if (id != 0x0C53BB6B)  // Cues ID
return E_FILE_FORMAT_INVALID;

pos += len; // consume ID
assert((segment_stop < 0) || (pos <= segment_stop));

// Read Size

if ((pos + 1) > avail) {
len = 1;
return E_BUFFER_NOT_FULL;
}

result = GetUIntLength(m_pReader, pos, len);

if (result < 0) // error
return static_cast<long>(result);

if (result > 0) // underflow (weird)
{
len = 1;
return E_BUFFER_NOT_FULL;
}

if ((segment_stop >= 0) && ((pos + len) > segment_stop))
return E_FILE_FORMAT_INVALID;

if ((pos + len) > avail)
return E_BUFFER_NOT_FULL;

const long long size = ReadUInt(m_pReader, pos, len);

if (size < 0) // error
return static_cast<long>(size);

if (size == 0) // weird, although technically not illegal
return 1; // done

pos += len; // consume length of size of element
assert((segment_stop < 0) || (pos <= segment_stop));

// Pos now points to start of payload

const long long element_stop = pos + size;

if ((segment_stop >= 0) && (element_stop > segment_stop))
return E_FILE_FORMAT_INVALID;

if ((total >= 0) && (element_stop > total))
return 1; // don't bother parsing anymore

len = static_cast<long>(size);

if (element_stop > avail)
return E_BUFFER_NOT_FULL;

const long long element_size = element_stop - element_start;


m_pCues =
new (std::nothrow) Cues(this, pos, size, element_start, element_size);
  assert(m_pCues);  // TODO

return 0;  // success
}
