static int set_register(pegasus_t *pegasus, __u16 indx, __u8 data)
{
int ret;

ret = usb_control_msg(pegasus->usb, usb_sndctrlpipe(pegasus->usb, 0),
PEGASUS_REQ_SET_REG, PEGASUS_REQT_WRITE, data,
			      indx, &data, 1, 1000);
if (ret < 0)
netif_dbg(pegasus, drv, pegasus->net,
"%s returned %d\n", __func__, ret);
return ret;
}
