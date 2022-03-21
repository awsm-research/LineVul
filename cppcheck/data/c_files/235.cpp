static void ProxyLocaltimeCallToBrowser(time_t input, struct tm* output,
char* timezone_out,
size_t timezone_out_len) {
base::Pickle request;
request.WriteInt(LinuxSandbox::METHOD_LOCALTIME);
request.WriteString(
std::string(reinterpret_cast<char*>(&input), sizeof(input)));

uint8_t reply_buf[512];
const ssize_t r = base::UnixDomainSocket::SendRecvMsg(
GetSandboxFD(), reply_buf, sizeof(reply_buf), NULL, request);
if (r == -1) {
    memset(output, 0, sizeof(struct tm));
return;
}

base::Pickle reply(reinterpret_cast<char*>(reply_buf), r);
base::PickleIterator iter(reply);
  std::string result;
  std::string timezone;
  if (!iter.ReadString(&result) ||
      !iter.ReadString(&timezone) ||
      result.size() != sizeof(struct tm)) {
memset(output, 0, sizeof(struct tm));
    return;
  }
  memcpy(output, result.data(), sizeof(struct tm));
  if (timezone_out_len) {
    const size_t copy_len = std::min(timezone_out_len - 1, timezone.size());
    memcpy(timezone_out, timezone.data(), copy_len);
    timezone_out[copy_len] = 0;
    output->tm_zone = timezone_out;
  } else {
    base::AutoLock lock(g_timezones_lock.Get());
    auto ret_pair = g_timezones.Get().insert(timezone);
    output->tm_zone = ret_pair.first->c_str();
}
}
