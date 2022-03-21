void ResourceDispatcherHostImpl::OnSSLCertificateError(
net::URLRequest* request,
const net::SSLInfo& ssl_info,
bool is_hsts_host) {
DCHECK(request);
ResourceRequestInfoImpl* info = ResourceRequestInfoImpl::ForRequest(request);
DCHECK(info);
GlobalRequestID request_id(info->GetChildID(), info->GetRequestID());
int render_process_id;
int render_view_id;
if(!info->GetAssociatedRenderView(&render_process_id, &render_view_id))
NOTREACHED();
  SSLManager::OnSSLCertificateError(ssl_delegate_weak_factory_.GetWeakPtr(),
      request_id, info->GetResourceType(), request->url(), render_process_id,
      render_view_id, ssl_info, is_hsts_host);
}
