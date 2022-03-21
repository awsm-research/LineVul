int main(int argc, char **argv) {
int frame_cnt = 0;
FILE *outfile = NULL;
vpx_codec_ctx_t codec;
VpxVideoReader *reader = NULL;
const VpxVideoInfo *info = NULL;
const VpxInterface *decoder = NULL;

exec_name = argv[0];

if (argc != 3)
die("Invalid number of arguments.");

reader = vpx_video_reader_open(argv[1]);
if (!reader)
die("Failed to open %s for reading.", argv[1]);

if (!(outfile = fopen(argv[2], "wb")))
die("Failed to open %s for writing.", argv[2]);

info = vpx_video_reader_get_info(reader);

decoder = get_vpx_decoder_by_fourcc(info->codec_fourcc);

if (!decoder)
die("Unknown input codec.");

  printf("Using %s\n", vpx_codec_iface_name(decoder->interface()));

  if (vpx_codec_dec_init(&codec, decoder->interface(), NULL, 0))
die_codec(&codec, "Failed to initialize decoder");

while (vpx_video_reader_read_frame(reader)) {
vpx_codec_iter_t iter = NULL;
vpx_image_t *img = NULL;
size_t frame_size = 0;
const unsigned char *frame = vpx_video_reader_get_frame(reader,
&frame_size);
if (vpx_codec_decode(&codec, frame, (unsigned int)frame_size, NULL, 0))
die_codec(&codec, "Failed to decode frame");

while ((img = vpx_codec_get_frame(&codec, &iter)) != NULL) {
unsigned char digest[16];

get_image_md5(img, digest);
print_md5(outfile, digest);
fprintf(outfile, "  img-%dx%d-%04d.i420\n",
img->d_w, img->d_h, ++frame_cnt);
}
}

printf("Processed %d frames.\n", frame_cnt);
if (vpx_codec_destroy(&codec))
die_codec(&codec, "Failed to destroy codec.");

vpx_video_reader_close(reader);

fclose(outfile);
return EXIT_SUCCESS;
}
