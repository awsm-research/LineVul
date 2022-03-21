static int em_sysenter(struct x86_emulate_ctxt *ctxt)
{
const struct x86_emulate_ops *ops = ctxt->ops;
struct desc_struct cs, ss;
u64 msr_data;
u16 cs_sel, ss_sel;
u64 efer = 0;

ops->get_msr(ctxt, MSR_EFER, &efer);
/* inject #GP if in real mode */
if (ctxt->mode == X86EMUL_MODE_REAL)
return emulate_gp(ctxt, 0);

/*
* Not recognized on AMD in compat mode (but is recognized in legacy
* mode).
*/
	if ((ctxt->mode == X86EMUL_MODE_PROT32) && (efer & EFER_LMA)
&& !vendor_intel(ctxt))
return emulate_ud(ctxt);

/* sysenter/sysexit have not been tested in 64bit mode. */
if (ctxt->mode == X86EMUL_MODE_PROT64)
return X86EMUL_UNHANDLEABLE;

setup_syscalls_segments(ctxt, &cs, &ss);

ops->get_msr(ctxt, MSR_IA32_SYSENTER_CS, &msr_data);
	switch (ctxt->mode) {
	case X86EMUL_MODE_PROT32:
		if ((msr_data & 0xfffc) == 0x0)
			return emulate_gp(ctxt, 0);
		break;
	case X86EMUL_MODE_PROT64:
		if (msr_data == 0x0)
			return emulate_gp(ctxt, 0);
		break;
	default:
		break;
	}

ctxt->eflags &= ~(EFLG_VM | EFLG_IF);
	cs_sel = (u16)msr_data;
	cs_sel &= ~SELECTOR_RPL_MASK;
ss_sel = cs_sel + 8;
	ss_sel &= ~SELECTOR_RPL_MASK;
	if (ctxt->mode == X86EMUL_MODE_PROT64 || (efer & EFER_LMA)) {
cs.d = 0;
cs.l = 1;
}

ops->set_segment(ctxt, cs_sel, &cs, 0, VCPU_SREG_CS);
ops->set_segment(ctxt, ss_sel, &ss, 0, VCPU_SREG_SS);

ops->get_msr(ctxt, MSR_IA32_SYSENTER_EIP, &msr_data);
	ctxt->_eip = msr_data;

ops->get_msr(ctxt, MSR_IA32_SYSENTER_ESP, &msr_data);
	*reg_write(ctxt, VCPU_REGS_RSP) = msr_data;

return X86EMUL_CONTINUE;
}
