long AudioTrack::Parse(Segment* pSegment, const Info& info,
long long element_start, long long element_size,
AudioTrack*& pResult) {
if (pResult)
return -1;

if (info.type != Track::kAudio)
return -1;

IMkvReader* const pReader = pSegment->m_pReader;

const Settings& s = info.settings;
assert(s.start >= 0);
assert(s.size >= 0);

long long pos = s.start;
assert(pos >= 0);

const long long stop = pos + s.size;

double rate = 8000.0; // MKV default
long long channels = 1;
long long bit_depth = 0;

while (pos < stop) {
long long id, size;

long status = ParseElementHeader(pReader, pos, stop, id, size);

if (status < 0) // error
return status;

if (id == 0x35) { // Sample Rate
status = UnserializeFloat(pReader, pos, size, rate);

if (status < 0)
return status;

if (rate <= 0)
return E_FILE_FORMAT_INVALID;
} else if (id == 0x1F) { // Channel Count
channels = UnserializeUInt(pReader, pos, size);

if (channels <= 0)
return E_FILE_FORMAT_INVALID;
} else if (id == 0x2264) { // Bit Depth
bit_depth = UnserializeUInt(pReader, pos, size);

if (bit_depth <= 0)
return E_FILE_FORMAT_INVALID;

}

pos += size;  // consume payload
    assert(pos <= stop);
}

  assert(pos == stop);

AudioTrack* const pTrack =
new (std::nothrow) AudioTrack(pSegment, element_start, element_size);

if (pTrack == NULL)
return -1; // generic error

const int status = info.Copy(pTrack->m_info);

if (status) {
delete pTrack;
return status;
}

pTrack->m_rate = rate;
pTrack->m_channels = channels;
pTrack->m_bitDepth = bit_depth;

pResult = pTrack;
return 0; // success
}
