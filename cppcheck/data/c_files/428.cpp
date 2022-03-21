bool AutofillDownloadManager::StartUploadRequest(
const FormStructure& form,
bool form_was_autofilled,
const FieldTypeSet& available_field_types) {
if (next_upload_request_ > base::Time::Now()) {
// We are in back-off mode: do not do the request.
VLOG(1) << "AutofillDownloadManager: Upload request is throttled.";
return false;
}

// Flip a coin to see if we should upload this form.
double upload_rate = form_was_autofilled ? GetPositiveUploadRate() :
GetNegativeUploadRate();
  if (base::RandDouble() > upload_rate) {
VLOG(1) << "AutofillDownloadManager: Upload request is ignored.";
// If we ever need notification that upload was skipped, add it here.
return false;
}

std::string form_xml;
if (!form.EncodeUploadRequest(available_field_types, form_was_autofilled,
&form_xml))
return false;

FormRequestData request_data;
request_data.form_signatures.push_back(form.FormSignature());
request_data.request_type = AutofillDownloadManager::REQUEST_UPLOAD;

return StartRequest(form_xml, request_data);
}
