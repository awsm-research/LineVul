void close_all_sockets(atransport* t) {
asocket* s;


/* this is a little gross, but since s->close() *will* modify
** the list out from under you, your options are limited.
*/
    adb_mutex_lock(&socket_list_lock);
restart:
for (s = local_socket_list.next; s != &local_socket_list; s = s->next) {
if (s->transport == t || (s->peer && s->peer->transport == t)) {
            local_socket_close_locked(s);
goto restart;
}
}
    adb_mutex_unlock(&socket_list_lock);
}
