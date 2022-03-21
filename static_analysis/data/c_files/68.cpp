int socket_create(uint16_t port)
{
int sfd = -1;
int yes = 1;
#ifdef WIN32
WSADATA wsa_data;
if (!wsa_init) {
if (WSAStartup(MAKEWORD(2,2), &wsa_data) != ERROR_SUCCESS) {
fprintf(stderr, "WSAStartup failed!\n");
ExitProcess(-1);
}
wsa_init = 1;
}
#endif
struct sockaddr_in saddr;

if (0 > (sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))) {
perror("socket()");
return -1;
}

if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(int)) == -1) {
perror("setsockopt()");
socket_close(sfd);
return -1;
}

memset((void *) &saddr, 0, sizeof(saddr));
saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
saddr.sin_port = htons(port);

if (0 > bind(sfd, (struct sockaddr *) &saddr, sizeof(saddr))) {
perror("bind()");
socket_close(sfd);
return -1;
}

if (listen(sfd, 1) == -1) {
perror("listen()");
socket_close(sfd);
return -1;
}

return sfd;
}
