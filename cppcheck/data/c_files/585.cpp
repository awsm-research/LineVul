void PrintWebViewHelper::UpdatePrintableSizeInPrintParameters(
WebFrame* frame,
WebNode* node,
ViewMsg_Print_Params* params) {
double content_width_in_points;
double content_height_in_points;
double margin_top_in_points;
double margin_right_in_points;
double margin_bottom_in_points;
double margin_left_in_points;
PrepareFrameAndViewForPrint prepare(*params, frame, node, frame->view());
PrintWebViewHelper::GetPageSizeAndMarginsInPoints(frame, 0, *params,
&content_width_in_points, &content_height_in_points,
&margin_top_in_points, &margin_right_in_points,
&margin_bottom_in_points, &margin_left_in_points);
#if defined(OS_MACOSX)
// On the Mac, the printable area is in points, don't do any scaling based
// on dpi.
int dpi = printing::kPointsPerInch;
#else
int dpi = static_cast<int>(params->dpi);
#endif  // defined(OS_MACOSX)
params->printable_size = gfx::Size(
static_cast<int>(ConvertUnitDouble(content_width_in_points,
printing::kPointsPerInch, dpi)),
static_cast<int>(ConvertUnitDouble(content_height_in_points,
printing::kPointsPerInch, dpi)));

params->page_size = gfx::Size(
      static_cast<int>(ConvertUnitDouble(content_width_in_points +
          margin_left_in_points + margin_right_in_points,
          printing::kPointsPerInch, dpi)),
      static_cast<int>(ConvertUnitDouble(content_height_in_points +
          margin_top_in_points + margin_bottom_in_points,
          printing::kPointsPerInch, dpi)));

params->margin_top = static_cast<int>(ConvertUnitDouble(
margin_top_in_points, printing::kPointsPerInch, dpi));
params->margin_left = static_cast<int>(ConvertUnitDouble(
margin_left_in_points, printing::kPointsPerInch, dpi));
}
