static void iwjpeg_scan_exif(struct iwjpegrcontext *rctx,
const iw_byte *d, size_t d_len)
{
struct iw_exif_state e;
iw_uint32 ifd;

if(d_len<8) return;

iw_zeromem(&e,sizeof(struct iw_exif_state));
e.d = d;
e.d_len = d_len;

e.endian = d[0]=='I' ? IW_ENDIAN_LITTLE : IW_ENDIAN_BIG;

	ifd = iw_get_ui32_e(&d[4],e.endian);

iwjpeg_scan_exif_ifd(rctx,&e,ifd);
}
