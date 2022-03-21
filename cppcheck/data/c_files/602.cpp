void PreconnectManager::Start(const GURL& url,
std::vector<PreconnectRequest> requests) {
DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
const std::string host = url.host();
if (preresolve_info_.find(host) != preresolve_info_.end())
return;

auto iterator_and_whether_inserted = preresolve_info_.emplace(
host, std::make_unique<PreresolveInfo>(url, requests.size()));
PreresolveInfo* info = iterator_and_whether_inserted.first->second.get();

for (auto request_it = requests.begin(); request_it != requests.end();
++request_it) {
    DCHECK(request_it->origin.GetOrigin() == request_it->origin);
PreresolveJobId job_id = preresolve_jobs_.Add(
std::make_unique<PreresolveJob>(std::move(*request_it), info));
queued_jobs_.push_back(job_id);
}

TryToLaunchPreresolveJobs();
}
