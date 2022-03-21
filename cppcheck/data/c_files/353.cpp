static int su3000_frontend_attach(struct dvb_usb_adapter *d)
{
	u8 obuf[3] = { 0xe, 0x80, 0 };
	u8 ibuf[] = { 0 };

	if (dvb_usb_generic_rw(d->dev, obuf, 3, ibuf, 1, 0) < 0)
err("command 0x0e transfer failed.");

	obuf[0] = 0xe;
	obuf[1] = 0x02;
	obuf[2] = 1;

	if (dvb_usb_generic_rw(d->dev, obuf, 3, ibuf, 1, 0) < 0)
err("command 0x0e transfer failed.");
msleep(300);

	obuf[0] = 0xe;
	obuf[1] = 0x83;
	obuf[2] = 0;

	if (dvb_usb_generic_rw(d->dev, obuf, 3, ibuf, 1, 0) < 0)
err("command 0x0e transfer failed.");

	obuf[0] = 0xe;
	obuf[1] = 0x83;
	obuf[2] = 1;

	if (dvb_usb_generic_rw(d->dev, obuf, 3, ibuf, 1, 0) < 0)
err("command 0x0e transfer failed.");

	obuf[0] = 0x51;

	if (dvb_usb_generic_rw(d->dev, obuf, 1, ibuf, 1, 0) < 0)
err("command 0x51 transfer failed.");

	d->fe_adap[0].fe = dvb_attach(ds3000_attach, &su3000_ds3000_config,
					&d->dev->i2c_adap);
	if (d->fe_adap[0].fe == NULL)
return -EIO;

	if (dvb_attach(ts2020_attach, d->fe_adap[0].fe,
&dw2104_ts2020_config,
				&d->dev->i2c_adap)) {
info("Attached DS3000/TS2020!");
return 0;
}

info("Failed to attach DS3000/TS2020!");
return -EIO;
}
