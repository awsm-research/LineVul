static int emulate_store_desc_ptr(struct x86_emulate_ctxt *ctxt,
void (*get)(struct x86_emulate_ctxt *ctxt,
struct desc_ptr *ptr))
{
struct desc_ptr desc_ptr;

if (ctxt->mode == X86EMUL_MODE_PROT64)
ctxt->op_bytes = 8;
get(ctxt, &desc_ptr);
if (ctxt->op_bytes == 2) {
ctxt->op_bytes = 4;
desc_ptr.address &= 0x00ffffff;
}
/* Disable writeback. */
ctxt->dst.type = OP_NONE;
	return segmented_write(ctxt, ctxt->dst.addr.mem,
			       &desc_ptr, 2 + ctxt->op_bytes);
}
