void ExtensionTtsController::Stop() {
  if (current_utterance_ && !current_utterance_->extension_id().empty()) {
    current_utterance_->profile()->GetExtensionEventRouter()->
        DispatchEventToExtension(
            current_utterance_->extension_id(),
            events::kOnStop,
            "[]",
            current_utterance_->profile(),
            GURL());
  } else {
    GetPlatformImpl()->clear_error();
    GetPlatformImpl()->StopSpeaking();
}

  if (current_utterance_)
    current_utterance_->set_error(kSpeechInterruptedError);
  FinishCurrentUtterance();
  ClearUtteranceQueue();
}
