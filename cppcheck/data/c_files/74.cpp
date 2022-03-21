void DOMStorageContextWrapper::Shutdown() {
  DCHECK(context_.get());
  mojo_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&LocalStorageContextMojo::ShutdownAndDelete,
                                base::Unretained(mojo_state_)));
  mojo_state_ = nullptr;
if (mojo_session_state_) {
mojo_task_runner_->PostTask(
FROM_HERE, base::BindOnce(&SessionStorageContextMojo::ShutdownAndDelete,
base::Unretained(mojo_session_state_)));
mojo_session_state_ = nullptr;
}
memory_pressure_listener_.reset();
  context_->task_runner()->PostShutdownBlockingTask(
      FROM_HERE, DOMStorageTaskRunner::PRIMARY_SEQUENCE,
      base::BindOnce(&DOMStorageContextImpl::Shutdown, context_));
}
