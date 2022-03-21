horDiff8(TIFF* tif, uint8* cp0, tmsize_t cc)
{
TIFFPredictorState* sp = PredictorState(tif);
tmsize_t stride = sp->stride;
unsigned char* cp = (unsigned char*) cp0;

	assert((cc%stride)==0);

if (cc > stride) {
cc -= stride;
/*
* Pipeline the most common cases.
*/
if (stride == 3) {
unsigned int r1, g1, b1;
unsigned int r2 = cp[0];
unsigned int g2 = cp[1];
unsigned  int b2 = cp[2];
do {
r1 = cp[3]; cp[3] = (unsigned char)((r1-r2)&0xff); r2 = r1;
g1 = cp[4]; cp[4] = (unsigned char)((g1-g2)&0xff); g2 = g1;
b1 = cp[5]; cp[5] = (unsigned char)((b1-b2)&0xff); b2 = b1;
cp += 3;
} while ((cc -= 3) > 0);
} else if (stride == 4) {
unsigned int r1, g1, b1, a1;
unsigned int r2 = cp[0];
unsigned int g2 = cp[1];
unsigned int b2 = cp[2];
unsigned int a2 = cp[3];
do {
r1 = cp[4]; cp[4] = (unsigned char)((r1-r2)&0xff); r2 = r1;
g1 = cp[5]; cp[5] = (unsigned char)((g1-g2)&0xff); g2 = g1;
b1 = cp[6]; cp[6] = (unsigned char)((b1-b2)&0xff); b2 = b1;
a1 = cp[7]; cp[7] = (unsigned char)((a1-a2)&0xff); a2 = a1;
cp += 4;
} while ((cc -= 4) > 0);
} else {
cp += cc - 1;
do {
REPEAT4(stride, cp[stride] = (unsigned char)((cp[stride] - cp[0])&0xff); cp--)
} while ((cc -= stride) > 0);
}
}
}
