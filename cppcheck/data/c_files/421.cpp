void DevToolsDownloadManagerDelegate::OnDownloadPathGenerated(
uint32_t download_id,
const content::DownloadTargetCallback& callback,
const base::FilePath& suggested_path) {
DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

callback.Run(suggested_path,
content::DownloadItem::TARGET_DISPOSITION_OVERWRITE,
               download::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
suggested_path.AddExtension(FILE_PATH_LITERAL(".crdownload")),
content::DOWNLOAD_INTERRUPT_REASON_NONE);
}
