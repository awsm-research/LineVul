void install_local_socket(asocket* s) {
    adb_mutex_lock(&socket_list_lock);

s->id = local_socket_next_id++;

// Socket ids should never be 0.
if (local_socket_next_id == 0) {
        local_socket_next_id = 1;
}

insert_local_socket(s, &local_socket_list);
    adb_mutex_unlock(&socket_list_lock);
}
