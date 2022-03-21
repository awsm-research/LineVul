void SSLManager::OnSSLCertificateError(
    base::WeakPtr<SSLErrorHandler::Delegate> delegate,
const content::GlobalRequestID& id,
const ResourceType::Type resource_type,
const GURL& url,
int render_process_id,
int render_view_id,
const net::SSLInfo& ssl_info,
bool fatal) {
DCHECK(delegate);
DVLOG(1) << "OnSSLCertificateError() cert_error: "
<< net::MapCertStatusToNetError(ssl_info.cert_status)
<< " id: " << id.child_id << "," << id.request_id
<< " resource_type: " << resource_type
<< " url: " << url.spec()
<< " render_process_id: " << render_process_id
<< " render_view_id: " << render_view_id
<< " cert_status: " << std::hex << ssl_info.cert_status;

// A certificate error occurred.  Construct a SSLCertErrorHandler object and
// hand it over to the UI thread for processing.
BrowserThread::PostTask(
BrowserThread::UI, FROM_HERE,
base::Bind(&SSLCertErrorHandler::Dispatch,
new SSLCertErrorHandler(delegate,
id,
resource_type,
url,
render_process_id,
render_view_id,
ssl_info,
fatal)));
}
