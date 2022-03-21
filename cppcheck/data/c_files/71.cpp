void MediaInterfaceProxy::CreateCdm(
media::mojom::ContentDecryptionModuleRequest request) {
DCHECK(thread_checker_.CalledOnValidThread());
  GetMediaInterfaceFactory()->CreateCdm(std::move(request));
}
