long ContentEncoding::ParseCompressionEntry(
    long long start,
    long long size,
    IMkvReader* pReader,
    ContentCompression* compression) {
assert(pReader);
assert(compression);

long long pos = start;
const long long stop = start + size;

bool valid = false;


while (pos < stop) {
long long id, size;
    const long status = ParseElementHeader(pReader,
                                           pos,
                                           stop,
                                           id,
                                           size);
    if (status < 0)  //error
return status;

if (id == 0x254) {
// ContentCompAlgo
long long algo = UnserializeUInt(pReader, pos, size);
if (algo < 0)
return E_FILE_FORMAT_INVALID;
compression->algo = algo;
valid = true;
} else if (id == 0x255) {
// ContentCompSettings
if (size <= 0)
return E_FILE_FORMAT_INVALID;

const size_t buflen = static_cast<size_t>(size);
typedef unsigned char* buf_t;
const buf_t buf = new (std::nothrow) unsigned char[buflen];

if (buf == NULL)
return -1;

      const int read_status = pReader->Read(pos, buflen, buf);
if (read_status) {
        delete [] buf;
return status;
}

compression->settings = buf;

compression->settings_len = buflen;
}

    pos += size;  //consume payload
assert(pos <= stop);
}

// ContentCompAlgo is mandatory
if (!valid)
return E_FILE_FORMAT_INVALID;


return 0;
}
