ResourceDispatcherHost::~ResourceDispatcherHost() {
AsyncResourceHandler::GlobalCleanup();
STLDeleteValues(&pending_requests_);

user_script_listener_->ShutdownMainThread();
  // Clear blocked requests if any left.
  // Note that we have to do this in 2 passes as we cannot call
  // CancelBlockedRequestsForRoute while iterating over
  // blocked_requests_map_, as it modifies it.
  std::set<ProcessRouteIDs> ids;
  for (BlockedRequestMap::const_iterator iter = blocked_requests_map_.begin();
       iter != blocked_requests_map_.end(); ++iter) {
    std::pair<std::set<ProcessRouteIDs>::iterator, bool> result =
        ids.insert(iter->first);
    // We should not have duplicates.
    DCHECK(result.second);
  }
  for (std::set<ProcessRouteIDs>::const_iterator iter = ids.begin();
       iter != ids.end(); ++iter) {
    CancelBlockedRequestsForRoute(iter->first, iter->second);
  }
}
