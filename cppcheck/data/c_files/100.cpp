int send_event (int fd, uint16_t type, uint16_t code, int32_t value)
{
struct uinput_event event;
BTIF_TRACE_DEBUG("%s type:%u code:%u value:%d", __FUNCTION__,
type, code, value);
memset(&event, 0, sizeof(event));
event.type  = type;

event.code  = code;
event.value = value;

    return write(fd, &event, sizeof(event));
}
