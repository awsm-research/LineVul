void flush_tlb_mm_range(struct mm_struct *mm, unsigned long start,
unsigned long end, unsigned long vmflag)
{
unsigned long addr;
/* do a global flush by default */
unsigned long base_pages_to_flush = TLB_FLUSH_ALL;

preempt_disable();
	if (current->active_mm != mm)
goto out;

if (!current->mm) {
leave_mm(smp_processor_id());
goto out;
}

if ((end != TLB_FLUSH_ALL) && !(vmflag & VM_HUGETLB))
base_pages_to_flush = (end - start) >> PAGE_SHIFT;

if (base_pages_to_flush > tlb_single_page_flush_ceiling) {
base_pages_to_flush = TLB_FLUSH_ALL;
count_vm_tlb_event(NR_TLB_LOCAL_FLUSH_ALL);
local_flush_tlb();
} else {
/* flush range by one by one 'invlpg' */
for (addr = start; addr < end;	addr += PAGE_SIZE) {
count_vm_tlb_event(NR_TLB_LOCAL_FLUSH_ONE);
__flush_tlb_single(addr);
}
}
trace_tlb_flush(TLB_LOCAL_MM_SHOOTDOWN, base_pages_to_flush);
out:
if (base_pages_to_flush == TLB_FLUSH_ALL) {
start = 0UL;
end = TLB_FLUSH_ALL;
}
if (cpumask_any_but(mm_cpumask(mm), smp_processor_id()) < nr_cpu_ids)
flush_tlb_others(mm_cpumask(mm), mm, start, end);
preempt_enable();
}
