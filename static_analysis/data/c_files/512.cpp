int WebContentsImpl::DownloadImage(
const GURL& url,
bool is_favicon,
uint32_t max_bitmap_size,
bool bypass_cache,
const WebContents::ImageDownloadCallback& callback) {
DCHECK_CURRENTLY_ON(BrowserThread::UI);
static int next_image_download_id = 0;
const image_downloader::ImageDownloaderPtr& mojo_image_downloader =
GetMainFrame()->GetMojoImageDownloader();
const int download_id = ++next_image_download_id;
if (!mojo_image_downloader) {
// If the renderer process is dead (i.e. crash, or memory pressure on
// Android), the downloader service will be invalid. Pre-Mojo, this would
// hang the callback indefinetly since the IPC would be dropped. Now,
// respond with a 400 HTTP error code to indicate that something went wrong.
BrowserThread::PostTask(
BrowserThread::UI, FROM_HERE,
        base::Bind(&WebContents::ImageDownloadCallback::Run,
                   base::Owned(new ImageDownloadCallback(callback)),
                   download_id, 400, url, std::vector<SkBitmap>(),
                   std::vector<gfx::Size>()));
return download_id;
}

image_downloader::DownloadRequestPtr req =
image_downloader::DownloadRequest::New();

req->url = mojo::String::From(url);
req->is_favicon = is_favicon;
req->max_bitmap_size = max_bitmap_size;
req->bypass_cache = bypass_cache;

mojo_image_downloader->DownloadImage(
      std::move(req),
      base::Bind(&DidDownloadImage, callback, download_id, url));
return download_id;
}
