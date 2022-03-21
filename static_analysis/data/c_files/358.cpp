bool Vp9Parser::ParseSuperframe() {
  const uint8_t* stream = stream_;
  off_t bytes_left = bytes_left_;
  DCHECK(frames_.empty());
  // Make sure we don't parse stream_ more than once.
  stream_ = nullptr;
  bytes_left_ = 0;
  if (bytes_left < 1)
    return false;
  // If this is a superframe, the last byte in the stream will contain the
  // superframe marker. If not, the whole buffer contains a single frame.
  uint8_t marker = *(stream + bytes_left - 1);
  if ((marker & 0xe0) != 0xc0) {
    frames_.push_back(FrameInfo(stream, bytes_left));
    return true;
  }
  DVLOG(1) << "Parsing a superframe";
  // The bytes immediately before the superframe marker constitute superframe
  // index, which stores information about sizes of each frame in it.
  // Calculate its size and set index_ptr to the beginning of it.
  size_t num_frames = (marker & 0x7) + 1;
  size_t mag = ((marker >> 3) & 0x3) + 1;
  off_t index_size = 2 + mag * num_frames;

  if (bytes_left < index_size)
return false;

  const uint8_t* index_ptr = stream + bytes_left - index_size;
  if (marker != *index_ptr)
    return false;
  ++index_ptr;
  bytes_left -= index_size;
  // Parse frame information contained in the index and add a pointer to and
  // size of each frame to frames_.
  for (size_t i = 0; i < num_frames; ++i) {
    uint32_t size = 0;
    for (size_t j = 0; j < mag; ++j) {
      size |= *index_ptr << (j * 8);
      ++index_ptr;
    }
    if (base::checked_cast<off_t>(size) > bytes_left) {
      DVLOG(1) << "Not enough data in the buffer for frame " << i;
      return false;
    }
    frames_.push_back(FrameInfo(stream, size));
    stream += size;
    bytes_left -= size;
    DVLOG(1) << "Frame " << i << ", size: " << size;
  }

return true;
}
