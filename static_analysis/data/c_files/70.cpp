size_t CancelableFileOperation(Function operation,
HANDLE file,
BufferType* buffer,
size_t length,
WaitableEvent* io_event,
WaitableEvent* cancel_event,
CancelableSyncSocket* socket,
DWORD timeout_in_ms) {
ThreadRestrictions::AssertIOAllowed();
// The buffer must be byte size or the length check won't make much sense.
COMPILE_ASSERT(sizeof(buffer[0]) == sizeof(char), incorrect_buffer_type);
DCHECK_GT(length, 0u);
DCHECK_LE(length, kMaxMessageLength);
DCHECK_NE(file, SyncSocket::kInvalidHandle);

// Track the finish time so we can calculate the timeout as data is read.
TimeTicks current_time, finish_time;
if (timeout_in_ms != INFINITE) {
current_time = TimeTicks::Now();
finish_time =
current_time + base::TimeDelta::FromMilliseconds(timeout_in_ms);
}

size_t count = 0;
do {
// The OVERLAPPED structure will be modified by ReadFile or WriteFile.
OVERLAPPED ol = { 0 };
ol.hEvent = io_event->handle();

const DWORD chunk = GetNextChunkSize(count, length);
// This is either the ReadFile or WriteFile call depending on whether
// we're receiving or sending data.
DWORD len = 0;
const BOOL operation_ok = operation(
file, static_cast<BufferType*>(buffer) + count, chunk, &len, &ol);
if (!operation_ok) {
if (::GetLastError() == ERROR_IO_PENDING) {
HANDLE events[] = { io_event->handle(), cancel_event->handle() };
const int wait_result = WaitForMultipleObjects(
            ARRAYSIZE_UNSAFE(events), events, FALSE,
timeout_in_ms == INFINITE ?
timeout_in_ms :
static_cast<DWORD>(
(finish_time - current_time).InMilliseconds()));
if (wait_result != WAIT_OBJECT_0 + 0) {
// CancelIo() doesn't synchronously cancel outstanding IO, only marks
// outstanding IO for cancellation. We must call GetOverlappedResult()
// below to ensure in flight writes complete before returning.
CancelIo(file);
}

// We set the |bWait| parameter to TRUE for GetOverlappedResult() to
// ensure writes are complete before returning.
if (!GetOverlappedResult(file, &ol, &len, TRUE))
len = 0;

if (wait_result == WAIT_OBJECT_0 + 1) {
DVLOG(1) << "Shutdown was signaled. Closing socket.";
socket->Close();
return count;
}

// Timeouts will be handled by the while() condition below since
// GetOverlappedResult() may complete successfully after CancelIo().
DCHECK(wait_result == WAIT_OBJECT_0 + 0 || wait_result == WAIT_TIMEOUT);
} else {
break;
}
}

count += len;

// Quit the operation if we can't write/read anymore.
if (len != chunk)
break;

// Since TimeTicks::Now() is expensive, only bother updating the time if we
// have more work to do.
if (timeout_in_ms != INFINITE && count < length)
current_time = base::TimeTicks::Now();
} while (count < length &&
(timeout_in_ms == INFINITE || current_time < finish_time));

return count;
}
