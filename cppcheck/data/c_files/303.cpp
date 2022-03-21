void SyncManager::SyncInternal::OnIPAddressChangedImpl() {
DCHECK(thread_checker_.CalledOnValidThread());
  // TODO(akalin): CheckServerReachable() can block, which may cause
  // jank if we try to shut down sync.  Fix this.
  connection_manager()->CheckServerReachable();
}
