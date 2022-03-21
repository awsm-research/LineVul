long Cluster::ParseSimpleBlock(long long block_size, long long& pos,
long& len) {
const long long block_start = pos;
const long long block_stop = pos + block_size;

IMkvReader* const pReader = m_pSegment->m_pReader;

long long total, avail;

long status = pReader->Length(&total, &avail);

if (status < 0) // error
return status;

assert((total < 0) || (avail <= total));

// parse track number

if ((pos + 1) > avail) {
len = 1;
return E_BUFFER_NOT_FULL;
}

long long result = GetUIntLength(pReader, pos, len);

if (result < 0) // error
return static_cast<long>(result);

if (result > 0) // weird
return E_BUFFER_NOT_FULL;

if ((pos + len) > block_stop)
return E_FILE_FORMAT_INVALID;

if ((pos + len) > avail)
return E_BUFFER_NOT_FULL;

const long long track = ReadUInt(pReader, pos, len);

if (track < 0) // error
return static_cast<long>(track);


if (track == 0)
return E_FILE_FORMAT_INVALID;

#if 0
    //TODO(matthewjheaney)
    //This turned out to be too conservative.  The problem is that
    //if we see a track header in the tracks element with an unsupported
    //track type, we throw that track header away, so it is not present
    //in the track map.  But even though we don't understand the track
    //header, there are still blocks in the cluster with that track
    //number.  It was our decision to ignore that track header, so it's
    //up to us to deal with blocks associated with that track -- we
    //cannot simply report an error since technically there's nothing
    //wrong with the file.
    
    //For now we go ahead and finish the parse, creating a block entry
    //for this block.  This is somewhat wasteful, because without a
    //track header there's nothing you can do with the block. What
    //we really need here is a special return value that indicates to
    //the caller that he should ignore this particular block, and
    //continue parsing.
    const Tracks* const pTracks = m_pSegment->GetTracks();
    assert(pTracks);
    const long tn = static_cast<long>(track);
    const Track* const pTrack = pTracks->GetTrackByNumber(tn);
    if (pTrack == NULL)
        return E_FILE_FORMAT_INVALID;
#endif
pos += len;  // consume track number

if ((pos + 2) > block_stop)
return E_FILE_FORMAT_INVALID;

if ((pos + 2) > avail) {
len = 2;
return E_BUFFER_NOT_FULL;
}

pos += 2; // consume timecode

if ((pos + 1) > block_stop)
return E_FILE_FORMAT_INVALID;

if ((pos + 1) > avail) {
len = 1;
return E_BUFFER_NOT_FULL;
}

unsigned char flags;

status = pReader->Read(pos, 1, &flags);

if (status < 0) { // error or underflow
len = 1;
return status;
}

++pos; // consume flags byte
assert(pos <= avail);

if (pos >= block_stop)
return E_FILE_FORMAT_INVALID;

const int lacing = int(flags & 0x06) >> 1;

if ((lacing != 0) && (block_stop > avail)) {
len = static_cast<long>(block_stop - pos);
return E_BUFFER_NOT_FULL;
}

status = CreateBlock(0x23, // simple block id
block_start, block_size,
0); // DiscardPadding

if (status != 0)
return status;

m_pos = block_stop;

return 0; // success
}
