void ServiceWorkerDevToolsAgentHost::AttachSession(DevToolsSession* session) {
if (state_ == WORKER_READY) {
if (sessions().size() == 1) {
BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
base::BindOnce(&SetDevToolsAttachedOnIO,
context_weak_, version_id_, true));
}
    // RenderProcessHost should not be null here, but even if it _is_ null,
    // session does not depend on the process to do messaging.
    session->SetRenderer(RenderProcessHost::FromID(worker_process_id_),
                         nullptr);
session->AttachToAgent(agent_ptr_);
}
session->AddHandler(base::WrapUnique(new protocol::InspectorHandler()));
session->AddHandler(base::WrapUnique(new protocol::NetworkHandler(GetId())));
session->AddHandler(base::WrapUnique(new protocol::SchemaHandler()));
}
