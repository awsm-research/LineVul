void ChromeMockRenderThread::OnMsgOpenChannelToExtension(
    int routing_id, const std::string& source_extension_id,
const std::string& target_extension_id,
    const std::string& channel_name, int* port_id) {
*port_id = 0;
}
