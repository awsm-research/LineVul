long Cluster::CreateBlock(
    long long id,
    long long pos,   //absolute pos of payload
    long long size,
    long long discard_padding)
{
    assert((id == 0x20) || (id == 0x23));  //BlockGroup or SimpleBlock
    if (m_entries_count < 0)  //haven't parsed anything yet
    {
        assert(m_entries == NULL);
        assert(m_entries_size == 0);
        m_entries_size = 1024;
        m_entries = new BlockEntry*[m_entries_size];
        m_entries_count = 0;
    }
    else
    {
        assert(m_entries);
        assert(m_entries_size > 0);
        assert(m_entries_count <= m_entries_size);
        if (m_entries_count >= m_entries_size)
        {
            const long entries_size = 2 * m_entries_size;
            BlockEntry** const entries = new BlockEntry*[entries_size];
            assert(entries);
            BlockEntry** src = m_entries;
            BlockEntry** const src_end = src + m_entries_count;
            BlockEntry** dst = entries;
            while (src != src_end)
                *dst++ = *src++;
            delete[] m_entries;
            m_entries = entries;
            m_entries_size = entries_size;
        }
}

    if (id == 0x20)  //BlockGroup ID
        return CreateBlockGroup(pos, size, discard_padding);
    else  //SimpleBlock ID
        return CreateSimpleBlock(pos, size);
}
