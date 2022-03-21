UserCloudPolicyManagerChromeOS::UserCloudPolicyManagerChromeOS(
scoped_ptr<CloudPolicyStore> store,
scoped_ptr<CloudExternalDataManager> external_data_manager,
const base::FilePath& component_policy_cache_path,
bool wait_for_policy_fetch,
base::TimeDelta initial_policy_fetch_timeout,
const scoped_refptr<base::SequencedTaskRunner>& task_runner,
const scoped_refptr<base::SequencedTaskRunner>& file_task_runner,
const scoped_refptr<base::SequencedTaskRunner>& io_task_runner)
: CloudPolicyManager(
PolicyNamespaceKey(dm_protocol::kChromeUserPolicyType, std::string()),
store.get(),
task_runner,
file_task_runner,
io_task_runner),
store_(store.Pass()),
external_data_manager_(external_data_manager.Pass()),
component_policy_cache_path_(component_policy_cache_path),
wait_for_policy_fetch_(wait_for_policy_fetch),
policy_fetch_timeout_(false, false) {
time_init_started_ = base::Time::Now();
  if (wait_for_policy_fetch_) {
policy_fetch_timeout_.Start(
FROM_HERE,
initial_policy_fetch_timeout,
base::Bind(&UserCloudPolicyManagerChromeOS::OnBlockingFetchTimeout,
base::Unretained(this)));
}
}
