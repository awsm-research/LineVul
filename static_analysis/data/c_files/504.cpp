status_t SampleTable::setTimeToSampleParams(
off64_t data_offset, size_t data_size) {
if (mTimeToSample != NULL || data_size < 8) {
return ERROR_MALFORMED;
}

uint8_t header[8];
if (mDataSource->readAt(
data_offset, header, sizeof(header)) < (ssize_t)sizeof(header)) {
return ERROR_IO;
}

if (U32_AT(header) != 0) {
// Expected version = 0, flags = 0.
return ERROR_MALFORMED;

}

mTimeToSampleCount = U32_AT(&header[4]);
    uint64_t allocSize = mTimeToSampleCount * 2 * sizeof(uint32_t);
if (allocSize > SIZE_MAX) {
return ERROR_OUT_OF_RANGE;
}
mTimeToSample = new uint32_t[mTimeToSampleCount * 2];

size_t size = sizeof(uint32_t) * mTimeToSampleCount * 2;
if (mDataSource->readAt(
data_offset + 8, mTimeToSample, size) < (ssize_t)size) {
return ERROR_IO;
}

for (uint32_t i = 0; i < mTimeToSampleCount * 2; ++i) {
mTimeToSample[i] = ntohl(mTimeToSample[i]);
}

return OK;
}
