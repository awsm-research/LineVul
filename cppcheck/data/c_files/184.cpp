void CuePoint::TrackPosition::Parse(IMkvReader* pReader, long long start_,
long long size_) {
const long long stop = start_ + size_;
long long pos = start_;

m_track = -1;
m_pos = -1;
m_block = 1; // default


while (pos < stop) {
long len;

    const long long id = ReadUInt(pReader, pos, len);
    assert(id >= 0);  // TODO
    assert((pos + len) <= stop);

pos += len;  // consume ID

const long long size = ReadUInt(pReader, pos, len);
    assert(size >= 0);
    assert((pos + len) <= stop);

pos += len;  // consume Size field
    assert((pos + size) <= stop);

if (id == 0x77)  // CueTrack ID
m_track = UnserializeUInt(pReader, pos, size);

else if (id == 0x71) // CueClusterPos ID
m_pos = UnserializeUInt(pReader, pos, size);

else if (id == 0x1378) // CueBlockNumber

m_block = UnserializeUInt(pReader, pos, size);

pos += size;  // consume payload
    assert(pos <= stop);
}

  assert(m_pos >= 0);
  assert(m_track > 0);
  // assert(m_block > 0);
}
