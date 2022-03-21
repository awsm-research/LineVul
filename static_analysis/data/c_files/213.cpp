static inline void set_socket_blocking(int s, int blocking)
{
int opts;
    opts = fcntl(s, F_GETFL);
if (opts<0) APPL_TRACE_ERROR("set blocking (%s)", strerror(errno));
if(blocking)
opts &= ~O_NONBLOCK;
else opts |= O_NONBLOCK;
    if (fcntl(s, F_SETFL, opts) < 0)
APPL_TRACE_ERROR("set blocking (%s)", strerror(errno));
}
