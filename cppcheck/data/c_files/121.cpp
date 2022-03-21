void Vp9Parser::ReadSegmentation() {
  segmentation_.update_map = false;
  segmentation_.update_data = false;

  segmentation_.enabled = reader_.ReadBool();
  if (!segmentation_.enabled)
return;

  segmentation_.update_map = reader_.ReadBool();
  if (segmentation_.update_map)
    ReadSegmentationMap();

  segmentation_.update_data = reader_.ReadBool();
  if (segmentation_.update_data)
    ReadSegmentationData();
}
