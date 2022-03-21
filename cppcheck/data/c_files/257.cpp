static void parse_input(h2o_http2_conn_t *conn)
{
size_t http2_max_concurrent_requests_per_connection = conn->super.ctx->globalconf->http2.max_concurrent_requests_per_connection;
int perform_early_exit = 0;

if (conn->num_streams.pull.half_closed + conn->num_streams.push.half_closed != http2_max_concurrent_requests_per_connection)
perform_early_exit = 1;

/* handle the input */
while (conn->state < H2O_HTTP2_CONN_STATE_IS_CLOSING && conn->sock->input->size != 0) {
if (perform_early_exit == 1 &&
conn->num_streams.pull.half_closed + conn->num_streams.push.half_closed == http2_max_concurrent_requests_per_connection)
goto EarlyExit;
/* process a frame */
const char *err_desc = NULL;
ssize_t ret = conn->_read_expect(conn, (uint8_t *)conn->sock->input->bytes, conn->sock->input->size, &err_desc);
if (ret == H2O_HTTP2_ERROR_INCOMPLETE) {
break;
} else if (ret < 0) {
if (ret != H2O_HTTP2_ERROR_PROTOCOL_CLOSE_IMMEDIATELY) {
enqueue_goaway(conn, (int)ret,
err_desc != NULL ? (h2o_iovec_t){(char *)err_desc, strlen(err_desc)} : (h2o_iovec_t){});
}
            close_connection(conn);
            return;
}
/* advance to the next frame */
h2o_buffer_consume(&conn->sock->input, ret);
}

if (!h2o_socket_is_reading(conn->sock))
h2o_socket_read_start(conn->sock, on_read);
    return;

EarlyExit:
if (h2o_socket_is_reading(conn->sock))
h2o_socket_read_stop(conn->sock);
}
