static int skt_write(int fd, const void *p, size_t len)
{
int sent;
struct pollfd pfd;

FNLOG();

pfd.fd = fd;
pfd.events = POLLOUT;


/* poll for 500 ms */

/* send time out */
    if (poll(&pfd, 1, 500) == 0)
return 0;

ts_log("skt_write", len, NULL);

    if ((sent = send(fd, p, len, MSG_NOSIGNAL)) == -1)
{
ERROR("write failed with errno=%d\n", errno);
return -1;
}

return sent;
}
