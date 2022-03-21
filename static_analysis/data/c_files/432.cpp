bool Vp9Parser::ParseUncompressedHeader(const uint8_t* stream,
                                        off_t frame_size,
                                        Vp9FrameHeader* fhdr) {
  reader_.Initialize(stream, frame_size);
  fhdr->data = stream;
  fhdr->frame_size = frame_size;

// frame marker
if (reader_.ReadLiteral(2) != 0x2)
return false;

fhdr->profile = ReadProfile();
if (fhdr->profile >= kVp9MaxProfile) {
DVLOG(1) << "Unsupported bitstream profile";
return false;
}

fhdr->show_existing_frame = reader_.ReadBool();
if (fhdr->show_existing_frame) {
fhdr->frame_to_show = reader_.ReadLiteral(3);
fhdr->show_frame = true;

if (!reader_.IsValid()) {
DVLOG(1) << "parser reads beyond the end of buffer";
return false;
}
fhdr->uncompressed_header_size = reader_.GetBytesRead();
return true;
}

fhdr->frame_type = static_cast<Vp9FrameHeader::FrameType>(reader_.ReadBool());
fhdr->show_frame = reader_.ReadBool();
fhdr->error_resilient_mode = reader_.ReadBool();

if (fhdr->IsKeyframe()) {
if (!VerifySyncCode())
return false;

if (!ReadBitDepthColorSpaceSampling(fhdr))
return false;

    fhdr->refresh_flags = 0xff;

ReadFrameSize(fhdr);
ReadDisplayFrameSize(fhdr);
} else {
if (!fhdr->show_frame)
fhdr->intra_only = reader_.ReadBool();

if (!fhdr->error_resilient_mode)
fhdr->reset_context = reader_.ReadLiteral(2);

if (fhdr->intra_only) {
if (!VerifySyncCode())
return false;

if (fhdr->profile > 0) {
if (!ReadBitDepthColorSpaceSampling(fhdr))
return false;
} else {
fhdr->bit_depth = 8;
fhdr->color_space = Vp9ColorSpace::BT_601;
fhdr->subsampling_x = fhdr->subsampling_y = 1;
}

      fhdr->refresh_flags = reader_.ReadLiteral(8);
ReadFrameSize(fhdr);
ReadDisplayFrameSize(fhdr);
} else {
      fhdr->refresh_flags = reader_.ReadLiteral(8);

for (size_t i = 0; i < kVp9NumRefsPerFrame; i++) {
fhdr->frame_refs[i] = reader_.ReadLiteral(kVp9NumRefFramesLog2);
fhdr->ref_sign_biases[i] = reader_.ReadBool();
}

if (!ReadFrameSizeFromRefs(fhdr))
return false;
ReadDisplayFrameSize(fhdr);

fhdr->allow_high_precision_mv = reader_.ReadBool();
fhdr->interp_filter = ReadInterpFilter();
}
}

if (fhdr->error_resilient_mode) {
fhdr->frame_parallel_decoding_mode = true;
} else {
fhdr->refresh_frame_context = reader_.ReadBool();
fhdr->frame_parallel_decoding_mode = reader_.ReadBool();
}

fhdr->frame_context_idx = reader_.ReadLiteral(2);

  if (fhdr->IsKeyframe() || fhdr->intra_only)
    SetupPastIndependence();
  ReadLoopFilter();
ReadQuantization(&fhdr->quant_params);
  ReadSegmentation();

ReadTiles(fhdr);

fhdr->first_partition_size = reader_.ReadLiteral(16);
if (fhdr->first_partition_size == 0) {
DVLOG(1) << "invalid header size";
return false;
}

if (!reader_.IsValid()) {
DVLOG(1) << "parser reads beyond the end of buffer";
return false;
}
fhdr->uncompressed_header_size = reader_.GetBytesRead();

  SetupSegmentationDequant(fhdr->quant_params);
  SetupLoopFilter();
  UpdateSlots(fhdr);
return true;
}
