static int complete_emulated_mmio(struct kvm_vcpu *vcpu)
{
struct kvm_run *run = vcpu->run;
struct kvm_mmio_fragment *frag;
unsigned len;

BUG_ON(!vcpu->mmio_needed);

/* Complete previous fragment */
frag = &vcpu->mmio_fragments[vcpu->mmio_cur_fragment];
len = min(8u, frag->len);
if (!vcpu->mmio_is_write)
memcpy(frag->data, run->mmio.data, len);

if (frag->len <= 8) {
/* Switch to the next fragment. */
frag++;
vcpu->mmio_cur_fragment++;
} else {
/* Go forward to the next mmio piece. */
frag->data += len;
frag->gpa += len;
frag->len -= len;
}

	if (vcpu->mmio_cur_fragment == vcpu->mmio_nr_fragments) {
vcpu->mmio_needed = 0;

/* FIXME: return into emulator if single-stepping.  */
if (vcpu->mmio_is_write)
return 1;
vcpu->mmio_read_completed = 1;
return complete_emulated_io(vcpu);
}

run->exit_reason = KVM_EXIT_MMIO;
run->mmio.phys_addr = frag->gpa;
if (vcpu->mmio_is_write)
memcpy(run->mmio.data, frag->data, min(8u, frag->len));
run->mmio.len = min(8u, frag->len);
run->mmio.is_write = vcpu->mmio_is_write;
vcpu->arch.complete_userspace_io = complete_emulated_mmio;
return 0;
}
