static int handle_vmptrst(struct kvm_vcpu *vcpu)
{
unsigned long exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
u32 vmx_instruction_info = vmcs_read32(VMX_INSTRUCTION_INFO);
gva_t vmcs_gva;
struct x86_exception e;

if (!nested_vmx_check_permission(vcpu))
return 1;

if (get_vmx_mem_address(vcpu, exit_qualification,
vmx_instruction_info, true, &vmcs_gva))
return 1;
	/* ok to use *_system, as hardware has verified cpl=0 *
if (kvm_write_guest_virt_system(&vcpu->arch.emulate_ctxt, vmcs_gva,
(void *)&to_vmx(vcpu)->nested.current_vmptr,
sizeof(u64), &e)) {
kvm_inject_page_fault(vcpu, &e);
return 1;
}
nested_vmx_succeed(vcpu);
return kvm_skip_emulated_instruction(vcpu);
}
