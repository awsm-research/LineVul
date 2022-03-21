static int i8042_start(struct serio *serio)
{
struct i8042_port *port = serio->port_data;

port->exists = true;
	mb();
return 0;
}
