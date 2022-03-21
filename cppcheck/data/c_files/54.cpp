void StoreAccumulatedContentLength(int received_content_length,
                                   int original_content_length,
                                   bool data_reduction_proxy_was_used) {
BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
base::Bind(&UpdateContentLengthPrefs,
received_content_length, original_content_length,
                 data_reduction_proxy_was_used));
}
