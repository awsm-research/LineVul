void AppCacheGroup::RemoveCache(AppCache* cache) {
DCHECK(cache->associated_hosts().empty());
if (cache == newest_complete_cache_) {
    CancelUpdate();
AppCache* tmp_cache = newest_complete_cache_;
newest_complete_cache_ = nullptr;
tmp_cache->set_owning_group(nullptr);  // may cause this group to be deleted
} else {
scoped_refptr<AppCacheGroup> protect(this);

Caches::iterator it =
std::find(old_caches_.begin(), old_caches_.end(), cache);
if (it != old_caches_.end()) {
AppCache* tmp_cache = *it;
old_caches_.erase(it);
tmp_cache->set_owning_group(nullptr);  // may cause group to be released
}

if (!is_obsolete() && old_caches_.empty() &&
!newly_deletable_response_ids_.empty()) {
storage_->DeleteResponses(manifest_url_, newly_deletable_response_ids_);
newly_deletable_response_ids_.clear();
}
}
}
