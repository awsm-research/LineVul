long Chapters::Parse()
{
    IMkvReader* const pReader = m_pSegment->m_pReader;
    long long pos = m_start;  // payload start
    const long long stop = pos + m_size;  // payload stop
    while (pos < stop)
    {
        long long id, size;
        long status = ParseElementHeader(
                        pReader,
                        pos,
                        stop,
                        id,
                        size);
        if (status < 0)  // error
            return status;
        if (size == 0)  // weird
            continue;
        if (id == 0x05B9)  // EditionEntry ID
        {
            status = ParseEdition(pos, size);
            if (status < 0)  // error
                return status;
        }
        pos += size;
        assert(pos <= stop);
}

    assert(pos == stop);
    return 0;
}
