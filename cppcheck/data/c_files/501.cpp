static void copyStereo8(
short *dst,
        const int *const *src,
unsigned nSamples,
unsigned /* nChannels */) {
for (unsigned i = 0; i < nSamples; ++i) {
*dst++ = src[0][i] << 8;
*dst++ = src[1][i] << 8;

}
}
