void HostPortAllocatorSession::OnSessionRequestDone(
    UrlFetcher* url_fetcher,
    const net::URLRequestStatus& status,
    int response_code,
    const std::string& response) {
  url_fetchers_.erase(url_fetcher);
  delete url_fetcher;

  if (response_code != net::HTTP_OK) {
LOG(WARNING) << "Received error when allocating relay session: "
                 << response_code;
TryCreateRelaySession();
return;
}

ReceiveSessionResponse(response);
}
