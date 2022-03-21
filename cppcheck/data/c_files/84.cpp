void MockWebRTCPeerConnectionHandler::setRemoteDescription(const WebRTCVoidRequest& request, const WebRTCSessionDescriptionDescriptor& remoteDescription)
{
    if (!remoteDescription.isNull() && remoteDescription.type() == "answer") {
        m_remoteDescription = remoteDescription;
        postTask(new RTCVoidRequestTask(this, request, true));
} else
        postTask(new RTCVoidRequestTask(this, request, false));
}
