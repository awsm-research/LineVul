static int kvm_vm_ioctl_set_pit2(struct kvm *kvm, struct kvm_pit_state2 *ps)
{
int start = 0;
u32 prev_legacy, cur_legacy;
mutex_lock(&kvm->arch.vpit->pit_state.lock);
prev_legacy = kvm->arch.vpit->pit_state.flags & KVM_PIT_FLAGS_HPET_LEGACY;
cur_legacy = ps->flags & KVM_PIT_FLAGS_HPET_LEGACY;
if (!prev_legacy && cur_legacy)
start = 1;
memcpy(&kvm->arch.vpit->pit_state.channels, &ps->channels,
sizeof(kvm->arch.vpit->pit_state.channels));
kvm->arch.vpit->pit_state.flags = ps->flags;
	kvm_pit_load_count(kvm, 0, kvm->arch.vpit->pit_state.channels[0].count, start);
mutex_unlock(&kvm->arch.vpit->pit_state.lock);
return 0;
}
