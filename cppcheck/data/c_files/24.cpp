static int cypress_open(struct tty_struct *tty, struct usb_serial_port *port)
{
struct cypress_private *priv = usb_get_serial_port_data(port);
struct usb_serial *serial = port->serial;
unsigned long flags;
int result = 0;

if (!priv->comm_is_ok)
return -EIO;

/* clear halts before open */
usb_clear_halt(serial->dev, 0x81);
usb_clear_halt(serial->dev, 0x02);

spin_lock_irqsave(&priv->lock, flags);
/* reset read/write statistics */
priv->bytes_in = 0;
priv->bytes_out = 0;
priv->cmd_count = 0;
priv->rx_flags = 0;
spin_unlock_irqrestore(&priv->lock, flags);

/* Set termios */
cypress_send(port);

if (tty)
cypress_set_termios(tty, port, &priv->tmp_termios);

/* setup the port and start reading from the device */
	if (!port->interrupt_in_urb) {
		dev_err(&port->dev, "%s - interrupt_in_urb is empty!\n",
			__func__);
		return -1;
	}
usb_fill_int_urb(port->interrupt_in_urb, serial->dev,
usb_rcvintpipe(serial->dev, port->interrupt_in_endpointAddress),
port->interrupt_in_urb->transfer_buffer,
port->interrupt_in_urb->transfer_buffer_length,
cypress_read_int_callback, port, priv->read_urb_interval);
result = usb_submit_urb(port->interrupt_in_urb, GFP_KERNEL);

if (result) {
dev_err(&port->dev,
"%s - failed submitting read urb, error %d\n",
__func__, result);
cypress_set_dead(port);
}

return result;
} /* cypress_open */
