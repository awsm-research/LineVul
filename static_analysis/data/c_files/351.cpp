long Cluster::HasBlockEntries(
const Segment* pSegment,
    long long off,  //relative to start of segment payload
    long long& pos,
    long& len)
{
    assert(pSegment);
    assert(off >= 0);  //relative to segment

    IMkvReader* const pReader = pSegment->m_pReader;

    long long total, avail;

    long status = pReader->Length(&total, &avail);

    if (status < 0)  //error
        return status;

    assert((total < 0) || (avail <= total));

    pos = pSegment->m_start + off;  //absolute

    if ((total >= 0) && (pos >= total))
        return 0;  //we don't even have a complete cluster

    const long long segment_stop =
        (pSegment->m_size < 0) ? -1 : pSegment->m_start + pSegment->m_size;

    long long cluster_stop = -1;  //interpreted later to mean "unknown size"

    {
        if ((pos + 1) > avail)
        {
            len = 1;
            return E_BUFFER_NOT_FULL;
        }
        long long result = GetUIntLength(pReader, pos, len);
        if (result < 0)  //error
            return static_cast<long>(result);
        if (result > 0)  //need more data
            return E_BUFFER_NOT_FULL;
        if ((segment_stop >= 0) && ((pos + len) > segment_stop))
            return E_FILE_FORMAT_INVALID;
        if ((total >= 0) && ((pos + len) > total))
            return 0;
        if ((pos + len) > avail)
            return E_BUFFER_NOT_FULL;
        const long long id = ReadUInt(pReader, pos, len);
        if (id < 0)  //error
            return static_cast<long>(id);
        if (id != 0x0F43B675)  //weird: not cluster ID
            return -1;         //generic error
        pos += len;  //consume Cluster ID field
        //read size field
        if ((pos + 1) > avail)
        {
            len = 1;
            return E_BUFFER_NOT_FULL;
        }
        result = GetUIntLength(pReader, pos, len);
        if (result < 0)  //error
            return static_cast<long>(result);
        if (result > 0)  //weird
            return E_BUFFER_NOT_FULL;
        if ((segment_stop >= 0) && ((pos + len) > segment_stop))
            return E_FILE_FORMAT_INVALID;
        if ((total >= 0) && ((pos + len) > total))
            return 0;
        if ((pos + len) > avail)
            return E_BUFFER_NOT_FULL;
        const long long size = ReadUInt(pReader, pos, len);
        if (size < 0)  //error
            return static_cast<long>(size);
        if (size == 0)
            return 0;  //cluster does not have entries
        pos += len;  //consume size field
        //pos now points to start of payload
        const long long unknown_size = (1LL << (7 * len)) - 1;
        if (size != unknown_size)
        {
            cluster_stop = pos + size;
            assert(cluster_stop >= 0);
            if ((segment_stop >= 0) && (cluster_stop > segment_stop))
                return E_FILE_FORMAT_INVALID;
            if ((total >= 0) && (cluster_stop > total))
                //return E_FILE_FORMAT_INVALID;  //too conservative
                return 0;  //cluster does not have any entries
        }
}

    for (;;)
    {
        if ((cluster_stop >= 0) && (pos >= cluster_stop))
            return 0;  //no entries detected

        if ((pos + 1) > avail)
        {
            len = 1;
            return E_BUFFER_NOT_FULL;
        }

        long long result = GetUIntLength(pReader, pos, len);

        if (result < 0)  //error
            return static_cast<long>(result);

        if (result > 0)  //need more data
            return E_BUFFER_NOT_FULL;

        if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
            return E_FILE_FORMAT_INVALID;

        if ((pos + len) > avail)
            return E_BUFFER_NOT_FULL;

        const long long id = ReadUInt(pReader, pos, len);

        if (id < 0)  //error
            return static_cast<long>(id);

        //This is the distinguished set of ID's we use to determine
        //that we have exhausted the sub-element's inside the cluster
        //whose ID we parsed earlier.

        if (id == 0x0F43B675)  //Cluster ID
            return 0;  //no entries found

        if (id == 0x0C53BB6B)  //Cues ID
            return 0;  //no entries found
        pos += len;  //consume id field
        if ((cluster_stop >= 0) && (pos >= cluster_stop))
            return E_FILE_FORMAT_INVALID;
        //read size field
        if ((pos + 1) > avail)
        {
            len = 1;
            return E_BUFFER_NOT_FULL;
        }
        result = GetUIntLength(pReader, pos, len);
        if (result < 0)  //error
            return static_cast<long>(result);
        if (result > 0)  //underflow
            return E_BUFFER_NOT_FULL;
        if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
            return E_FILE_FORMAT_INVALID;
        if ((pos + len) > avail)
            return E_BUFFER_NOT_FULL;
        const long long size = ReadUInt(pReader, pos, len);
        if (size < 0)  //error
            return static_cast<long>(size);
        pos += len;  //consume size field
        //pos now points to start of payload
        if ((cluster_stop >= 0) && (pos > cluster_stop))
            return E_FILE_FORMAT_INVALID;
        if (size == 0)  //weird
            continue;
        const long long unknown_size = (1LL << (7 * len)) - 1;
        if (size == unknown_size)
            return E_FILE_FORMAT_INVALID;  //not supported inside cluster
        if ((cluster_stop >= 0) && ((pos + size) > cluster_stop))
            return E_FILE_FORMAT_INVALID;
        if (id == 0x20)  //BlockGroup ID
            return 1;    //have at least one entry
        if (id == 0x23)  //SimpleBlock ID
            return 1;    //have at least one entry
        pos += size;  //consume payload
        assert((cluster_stop < 0) || (pos <= cluster_stop));
}
}
