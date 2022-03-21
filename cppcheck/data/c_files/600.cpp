long long Segment::CreateInstance(
    IMkvReader* pReader,
    long long pos,
    Segment*& pSegment)
{
    assert(pReader);
    assert(pos >= 0);

    pSegment = NULL;

    long long total, available;

    const long status = pReader->Length(&total, &available);

    if (status < 0) //error
        return status;

    if (available < 0)
return -1;

    if ((total >= 0) && (available > total))
return -1;

    //I would assume that in practice this loop would execute
    //exactly once, but we allow for other elements (e.g. Void)
    //to immediately follow the EBML header.  This is fine for
    //the source filter case (since the entire file is available),
    //but in the splitter case over a network we should probably
    //just give up early.  We could for example decide only to
    //execute this loop a maximum of, say, 10 times.
    //TODO:
    //There is an implied "give up early" by only parsing up
    //to the available limit.  We do do that, but only if the
    //total file size is unknown.  We could decide to always
    //use what's available as our limit (irrespective of whether
    //we happen to know the total file length).  This would have
    //as its sense "parse this much of the file before giving up",
    //which a slightly different sense from "try to parse up to
    //10 EMBL elements before giving up".

    for (;;)
    {
        if ((total >= 0) && (pos >= total))
            return E_FILE_FORMAT_INVALID;
        //Read ID
        long len;
        long long result = GetUIntLength(pReader, pos, len);
        if (result)  //error, or too few available bytes
            return result;
        if ((total >= 0) && ((pos + len) > total))
            return E_FILE_FORMAT_INVALID;
        if ((pos + len) > available)
            return pos + len;
        const long long idpos = pos;
        const long long id = ReadUInt(pReader, pos, len);
        if (id < 0)  //error
            return id;
        pos += len;  //consume ID
        //Read Size
        result = GetUIntLength(pReader, pos, len);
        if (result)  //error, or too few available bytes
            return result;
        if ((total >= 0) && ((pos + len) > total))
            return E_FILE_FORMAT_INVALID;
        if ((pos + len) > available)
            return pos + len;
        long long size = ReadUInt(pReader, pos, len);
        if (size < 0)  //error
            return size;
        pos += len;  //consume length of size of element
        //Pos now points to start of payload
        //Handle "unknown size" for live streaming of webm files.
        const long long unknown_size = (1LL << (7 * len)) - 1;
        if (id == 0x08538067)  //Segment ID
        {
            if (size == unknown_size)
                size = -1;
            else if (total < 0)
                size = -1;
            else if ((pos + size) > total)
                size = -1;
            pSegment = new (std::nothrow) Segment(
                                            pReader,
                                            idpos,
                                            //elem_size
                                            pos,
                                            size);
            if (pSegment == 0)
                return -1;  //generic error
            return 0;    //success
        }
        if (size == unknown_size)
            return E_FILE_FORMAT_INVALID;
        if ((total >= 0) && ((pos + size) > total))
            return E_FILE_FORMAT_INVALID;
        if ((pos + size) > available)
            return pos + size;
        pos += size;  //consume payload
    }
}
