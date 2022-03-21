void ResourcePrefetchPredictor::LearnOrigins(
const std::string& host,
const GURL& main_frame_origin,
    const std::map<GURL, OriginRequestSummary>& summaries) {
DCHECK_CURRENTLY_ON(BrowserThread::UI);
if (host.size() > ResourcePrefetchPredictorTables::kMaxStringLength)
return;

OriginData data;
bool exists = origin_data_->TryGetData(host, &data);
if (!exists) {
data.set_host(host);
data.set_last_visit_time(base::Time::Now().ToInternalValue());
size_t origins_size = summaries.size();
auto ordered_origins =
std::vector<const OriginRequestSummary*>(origins_size);
for (const auto& kv : summaries) {
size_t index = kv.second.first_occurrence;
DCHECK_LT(index, origins_size);
ordered_origins[index] = &kv.second;
}

for (const OriginRequestSummary* summary : ordered_origins) {
auto* origin_to_add = data.add_origins();
InitializeOriginStatFromOriginRequestSummary(origin_to_add, *summary);
}
} else {
data.set_last_visit_time(base::Time::Now().ToInternalValue());

    std::map<GURL, int> old_index;
int old_size = static_cast<int>(data.origins_size());
for (int i = 0; i < old_size; ++i) {
bool is_new =
          old_index.insert({GURL(data.origins(i).origin()), i}).second;
DCHECK(is_new);
}

// Update the old origins.
for (int i = 0; i < old_size; ++i) {
auto* old_origin = data.mutable_origins(i);
      GURL origin(old_origin->origin());
auto it = summaries.find(origin);
if (it == summaries.end()) {
// miss
old_origin->set_number_of_misses(old_origin->number_of_misses() + 1);
old_origin->set_consecutive_misses(old_origin->consecutive_misses() +
1);
} else {
// hit: update.
const auto& new_origin = it->second;
old_origin->set_always_access_network(new_origin.always_access_network);
old_origin->set_accessed_network(new_origin.accessed_network);

int position = new_origin.first_occurrence + 1;
int total =
old_origin->number_of_hits() + old_origin->number_of_misses();
old_origin->set_average_position(
((old_origin->average_position() * total) + position) /
(total + 1));
old_origin->set_number_of_hits(old_origin->number_of_hits() + 1);
old_origin->set_consecutive_misses(0);
}
}

// Add new origins.
for (const auto& kv : summaries) {
if (old_index.find(kv.first) != old_index.end())
continue;

auto* origin_to_add = data.add_origins();
InitializeOriginStatFromOriginRequestSummary(origin_to_add, kv.second);
}
}

// Trim and Sort.
ResourcePrefetchPredictorTables::TrimOrigins(&data,
config_.max_consecutive_misses);
ResourcePrefetchPredictorTables::SortOrigins(&data, main_frame_origin.spec());
if (data.origins_size() > static_cast<int>(config_.max_origins_per_entry)) {
data.mutable_origins()->DeleteSubrange(
config_.max_origins_per_entry,
data.origins_size() - config_.max_origins_per_entry);
}

// Update the database.
if (data.origins_size() == 0)
origin_data_->DeleteData({host});
else
origin_data_->UpdateData(host, data);
}
