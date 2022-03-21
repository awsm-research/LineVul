static int sh_op(RAnal *anal, RAnalOp *op, ut64 addr, const ut8 *data, int len) {
ut8 op_MSB,op_LSB;
int ret;
	if (!data)
return 0;
memset (op, '\0', sizeof (RAnalOp));
op->addr = addr;
op->type = R_ANAL_OP_TYPE_UNK;
op->jump = op->fail = -1;
op->ptr = op->val = -1;

op->size = 2;

op_MSB = anal->big_endian? data[0]: data[1];
op_LSB = anal->big_endian? data[1]: data[0];
ret =  first_nibble_decode[(op_MSB>>4) & 0x0F](anal, op, (ut16)(op_MSB<<8 | op_LSB));
return ret;
}
