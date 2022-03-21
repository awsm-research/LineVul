bool ClipboardUtil::HasFilenames(IDataObject* data_object) {
DCHECK(data_object);
  return HasData(data_object, Clipboard::GetCFHDropFormatType());
}
