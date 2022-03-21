int PDFiumEngine::GetVisiblePageIndex(FPDF_PAGE page) {
  for (int page_index : visible_pages_) {
if (pages_[page_index]->GetPage() == page)
return page_index;
}
return -1;
}
