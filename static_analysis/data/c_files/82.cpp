status_t SampleTable::setTimeToSampleParams(
off64_t data_offset, size_t data_size) {
    if (!mTimeToSample.empty() || data_size < 8) {
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
if ((uint64_t)mTimeToSampleCount >
(uint64_t)UINT32_MAX / (2 * sizeof(uint32_t))) {
// Choose this bound because
// 1) 2 * sizeof(uint32_t) is the amount of memory needed for one
//    time-to-sample entry in the time-to-sample table.
// 2) mTimeToSampleCount is the number of entries of the time-to-sample
//    table.
// 3) We hope that the table size does not exceed UINT32_MAX.
ALOGE("  Error: Time-to-sample table size too large.");

return ERROR_OUT_OF_RANGE;
}

// Note: At this point, we know that mTimeToSampleCount * 2 will not
// overflow because of the above condition.
if (!mDataSource->getVector(data_offset + 8, &mTimeToSample,
mTimeToSampleCount * 2)) {
ALOGE("  Error: Incomplete data read for time-to-sample table.");
return ERROR_IO;
}


for (size_t i = 0; i < mTimeToSample.size(); ++i) {
mTimeToSample.editItemAt(i) = ntohl(mTimeToSample[i]);
}
return OK;
}
