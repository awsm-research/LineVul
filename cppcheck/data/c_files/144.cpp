long ContentEncoding::ParseContentEncodingEntry(long long start,
                                                long long size,
IMkvReader* pReader) {
assert(pReader);

long long pos = start;
const long long stop = start + size;

// Count ContentCompression and ContentEncryption elements.
int compression_count = 0;
int encryption_count = 0;


while (pos < stop) {
long long id, size;
    const long status = ParseElementHeader(pReader,
                                           pos,
                                           stop,
                                           id,
                                           size);
    if (status < 0)  //error
return status;

if (id == 0x1034)  // ContentCompression ID
++compression_count;


if (id == 0x1035)  // ContentEncryption ID
++encryption_count;

    pos += size;  //consume payload
assert(pos <= stop);
}

if (compression_count <= 0 && encryption_count <= 0)
return -1;


if (compression_count > 0) {
compression_entries_ =
        new (std::nothrow) ContentCompression*[compression_count];
if (!compression_entries_)
return -1;
compression_entries_end_ = compression_entries_;
}


if (encryption_count > 0) {
encryption_entries_ =
        new (std::nothrow) ContentEncryption*[encryption_count];
if (!encryption_entries_) {
      delete [] compression_entries_;
return -1;
}
encryption_entries_end_ = encryption_entries_;
}


pos = start;
while (pos < stop) {
long long id, size;
    long status = ParseElementHeader(pReader,
                                     pos,
                                     stop,
                                     id,
                                     size);
    if (status < 0)  //error
return status;

if (id == 0x1031) {
// ContentEncodingOrder
encoding_order_ = UnserializeUInt(pReader, pos, size);
} else if (id == 0x1032) {
// ContentEncodingScope
encoding_scope_ = UnserializeUInt(pReader, pos, size);
if (encoding_scope_ < 1)
return -1;
} else if (id == 0x1033) {
// ContentEncodingType
encoding_type_ = UnserializeUInt(pReader, pos, size);

} else if (id == 0x1034) {
// ContentCompression ID
ContentCompression* const compression =
        new (std::nothrow) ContentCompression();
if (!compression)
return -1;

status = ParseCompressionEntry(pos, size, pReader, compression);
if (status) {
delete compression;
return status;
}
*compression_entries_end_++ = compression;
} else if (id == 0x1035) {
// ContentEncryption ID
ContentEncryption* const encryption =
new (std::nothrow) ContentEncryption();
if (!encryption)
return -1;

status = ParseEncryptionEntry(pos, size, pReader, encryption);
if (status) {
delete encryption;
return status;
}

*encryption_entries_end_++ = encryption;
}

    pos += size;  //consume payload
assert(pos <= stop);
}

assert(pos == stop);

return 0;
}
