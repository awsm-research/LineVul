void DriveFileStreamReader::InitializeAfterGetFileContentByPathInitialized(
const base::FilePath& drive_file_path,
const InitializeCompletionCallback& callback,
FileError error,
scoped_ptr<DriveEntryProto> entry,
const base::FilePath& local_cache_file_path) {
DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

if (error != FILE_ERROR_OK) {
callback.Run(error, scoped_ptr<DriveEntryProto>());
return;
}
DCHECK(entry);

if (local_cache_file_path.empty()) {
// The file is not cached, and being downloaded.
reader_proxy_.reset(
new internal::NetworkReaderProxy(
            entry->file_info().size(),
base::Bind(&internal::CancelGetFile,
drive_file_system_getter_, drive_file_path)));
callback.Run(FILE_ERROR_OK, entry.Pass());
return;
}

// Otherwise, open the stream for file.
scoped_ptr<net::FileStream> file_stream(new net::FileStream(NULL));
net::FileStream* file_stream_ptr = file_stream.get();
net::CompletionCallback open_completion_callback = base::Bind(
&DriveFileStreamReader::InitializeAfterLocalFileOpen,
weak_ptr_factory_.GetWeakPtr(),
callback,
base::Passed(&entry),
base::Passed(&file_stream));
int result = file_stream_ptr->Open(
local_cache_file_path,
base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ |
base::PLATFORM_FILE_ASYNC,
open_completion_callback);

if (result == net::ERR_IO_PENDING) {
// If the result ERR_IO_PENDING, the callback will be invoked later.
// Do nothing here.
return;
}

open_completion_callback.Run(result);
}
