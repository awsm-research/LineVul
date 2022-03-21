bool UnprivilegedProcessDelegate::CreateConnectedIpcChannel(
    const std::string& channel_name,
    IPC::Listener* delegate,
    ScopedHandle* client_out,
    scoped_ptr<IPC::ChannelProxy>* server_out) {
  // Create the server end of the channel.
  scoped_ptr<IPC::ChannelProxy> server;
  if (!CreateIpcChannel(channel_name, kDaemonIpcSecurityDescriptor,
                        io_task_runner_, delegate, &server)) {
    return false;
  }
  // Convert the channel name to the pipe name.
  std::string pipe_name(kChromePipeNamePrefix);
  pipe_name.append(channel_name);
  SECURITY_ATTRIBUTES security_attributes;
  security_attributes.nLength = sizeof(security_attributes);
  security_attributes.lpSecurityDescriptor = NULL;
  security_attributes.bInheritHandle = TRUE;
  // Create the client end of the channel. This code should match the code in
  // IPC::Channel.
  ScopedHandle client;
  client.Set(CreateFile(UTF8ToUTF16(pipe_name).c_str(),
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        &security_attributes,
                        OPEN_EXISTING,
                        SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION |
                            FILE_FLAG_OVERLAPPED,
                        NULL));
  if (!client.IsValid())
    return false;
  *client_out = client.Pass();
  *server_out = server.Pass();
  return true;
}
