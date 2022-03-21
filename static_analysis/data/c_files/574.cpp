build_config(char *prefix, struct server *server)
{
char *path    = NULL;
int path_size = strlen(prefix) + strlen(server->port) + 20;

path = ss_malloc(path_size);
snprintf(path, path_size, "%s/.shadowsocks_%s.conf", prefix, server->port);
FILE *f = fopen(path, "w+");
if (f == NULL) {
if (verbose) {
LOGE("unable to open config file");
}
ss_free(path);
return;
}
fprintf(f, "{\n");
fprintf(f, "\"server_port\":%d,\n", atoi(server->port));
fprintf(f, "\"password\":\"%s\"", server->password);
    if (server->fast_open[0]) fprintf(f, ",\n\"fast_open\": %s", server->fast_open);
    if (server->mode)   fprintf(f, ",\n\"mode\":\"%s\"", server->mode);
    if (server->method) fprintf(f, ",\n\"method\":\"%s\"", server->method);
    if (server->plugin) fprintf(f, ",\n\"plugin\":\"%s\"", server->plugin);
    if (server->plugin_opts) fprintf(f, ",\n\"plugin_opts\":\"%s\"", server->plugin_opts);
fprintf(f, "\n}\n");
fclose(f);
ss_free(path);
}
