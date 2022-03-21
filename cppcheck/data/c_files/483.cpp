static int cx24116_send_diseqc_msg(struct dvb_frontend *fe,
struct dvb_diseqc_master_cmd *d)
{
struct cx24116_state *state = fe->demodulator_priv;
int i, ret;

/* Dump DiSEqC message */
if (debug) {
printk(KERN_INFO "cx24116: %s(", __func__);
for (i = 0 ; i < d->msg_len ;) {
printk(KERN_INFO "0x%02x", d->msg[i]);
if (++i < d->msg_len)
printk(KERN_INFO ", ");
}
printk(") toneburst=%d\n", toneburst);
}

	/* Validate length *
	if (d->msg_len > (CX24116_ARGLEN - CX24116_DISEQC_MSGOFS))
		return -EINVAL;
/* DiSEqC message */
for (i = 0; i < d->msg_len; i++)
state->dsec_cmd.args[CX24116_DISEQC_MSGOFS + i] = d->msg[i];

/* DiSEqC message length */
state->dsec_cmd.args[CX24116_DISEQC_MSGLEN] = d->msg_len;

/* Command length */
state->dsec_cmd.len = CX24116_DISEQC_MSGOFS +
state->dsec_cmd.args[CX24116_DISEQC_MSGLEN];

/* DiSEqC toneburst */
if (toneburst == CX24116_DISEQC_MESGCACHE)
/* Message is cached */
return 0;

else if (toneburst == CX24116_DISEQC_TONEOFF)
/* Message is sent without burst */
state->dsec_cmd.args[CX24116_DISEQC_BURST] = 0;

else if (toneburst == CX24116_DISEQC_TONECACHE) {
/*
* Message is sent with derived else cached burst
*
* WRITE PORT GROUP COMMAND 38
*
* 0/A/A: E0 10 38 F0..F3
* 1/B/B: E0 10 38 F4..F7
* 2/C/A: E0 10 38 F8..FB
* 3/D/B: E0 10 38 FC..FF
*
* databyte[3]= 8421:8421
*              ABCD:WXYZ
*              CLR :SET
*
*              WX= PORT SELECT 0..3    (X=TONEBURST)
*              Y = VOLTAGE             (0=13V, 1=18V)
*              Z = BAND                (0=LOW, 1=HIGH(22K))
*/
if (d->msg_len >= 4 && d->msg[2] == 0x38)
state->dsec_cmd.args[CX24116_DISEQC_BURST] =
((d->msg[3] & 4) >> 2);
if (debug)
dprintk("%s burst=%d\n", __func__,
state->dsec_cmd.args[CX24116_DISEQC_BURST]);
}

/* Wait for LNB ready */
ret = cx24116_wait_for_lnb(fe);
if (ret != 0)
return ret;

/* Wait for voltage/min repeat delay */
msleep(100);

/* Command */
ret = cx24116_cmd_execute(fe, &state->dsec_cmd);
if (ret != 0)
return ret;
/*
* Wait for send
*
* Eutelsat spec:
* >15ms delay          + (XXX determine if FW does this, see set_tone)
*  13.5ms per byte     +
* >15ms delay          +
*  12.5ms burst        +
* >15ms delay            (XXX determine if FW does this, see set_tone)
*/
msleep((state->dsec_cmd.args[CX24116_DISEQC_MSGLEN] << 4) +
((toneburst == CX24116_DISEQC_TONEOFF) ? 30 : 60));

return 0;
}
