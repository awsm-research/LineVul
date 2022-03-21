static void add_bytes_c(uint8_t *dst, uint8_t *src, int w){
long i;
    for(i=0; i<=w-sizeof(long); i+=sizeof(long)){
long a = *(long*)(src+i);
long b = *(long*)(dst+i);
*(long*)(dst+i) = ((a&pb_7f) + (b&pb_7f)) ^ ((a^b)&pb_80);
}
for(; i<w; i++)
dst[i+0] += src[i+0];
}
