void ScreenRecorder::DoAddConnection(
    scoped_refptr<ConnectionToClient> connection) {
  DCHECK(network_loop_->BelongsToCurrentThread());
  connections_.push_back(connection);
}
