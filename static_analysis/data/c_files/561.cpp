void SpeechRecognitionManagerImpl::RecognitionAllowedCallback(int session_id,
bool ask_user,
bool is_allowed) {
DCHECK_CURRENTLY_ON(BrowserThread::IO);

auto iter = sessions_.find(session_id);
if (iter == sessions_.end())
return;

Session* session = iter->second.get();

if (session->abort_requested)
return;

if (ask_user) {
SpeechRecognitionSessionContext& context = session->context;
context.label = media_stream_manager_->MakeMediaAccessRequest(
        context.render_process_id, context.render_frame_id, session_id,
        StreamControls(true, false), context.security_origin,
base::BindOnce(
&SpeechRecognitionManagerImpl::MediaRequestPermissionCallback,
weak_factory_.GetWeakPtr(), session_id));
return;
}

if (is_allowed) {
base::ThreadTaskRunnerHandle::Get()->PostTask(
FROM_HERE,
base::BindOnce(&SpeechRecognitionManagerImpl::DispatchEvent,
weak_factory_.GetWeakPtr(), session_id, EVENT_START));
} else {
OnRecognitionError(
session_id, blink::mojom::SpeechRecognitionError(
blink::mojom::SpeechRecognitionErrorCode::kNotAllowed,
blink::mojom::SpeechAudioErrorDetails::kNone));
base::ThreadTaskRunnerHandle::Get()->PostTask(
FROM_HERE,
base::BindOnce(&SpeechRecognitionManagerImpl::DispatchEvent,
weak_factory_.GetWeakPtr(), session_id, EVENT_ABORT));
}
}
