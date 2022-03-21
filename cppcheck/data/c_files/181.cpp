void FileSystemOperation::GetUsageAndQuotaThenRunTask(
const GURL& origin, FileSystemType type,
const base::Closure& task,
const base::Closure& error_callback) {
quota::QuotaManagerProxy* quota_manager_proxy =
file_system_context()->quota_manager_proxy();
if (!quota_manager_proxy ||
!file_system_context()->GetQuotaUtil(type)) {
// If we don't have the quota manager or the requested filesystem type
// does not support quota, we should be able to let it go.
operation_context_.set_allowed_bytes_growth(kint64max);
task.Run();
return;
}

TaskParamsForDidGetQuota params;
params.origin = origin;
params.type = type;
params.task = task;
params.error_callback = error_callback;

DCHECK(quota_manager_proxy);
DCHECK(quota_manager_proxy->quota_manager());
quota_manager_proxy->quota_manager()->GetUsageAndQuota(
origin,
FileSystemTypeToQuotaStorageType(type),
base::Bind(&FileSystemOperation::DidGetUsageAndQuotaAndRunTask,
                 base::Unretained(this), params));
}
