static struct page *alloc_huge_page(struct vm_area_struct *vma,
unsigned long addr, int avoid_reserve)
{
struct hstate *h = hstate_vma(vma);
struct page *page;
	struct address_space *mapping = vma->vm_file->f_mapping;
	struct inode *inode = mapping->host;
long chg;

/*
	 * Processes that did not create the mapping will have no reserves and
	 * will not have accounted against quota. Check that the quota can be
	 * made before satisfying the allocation
	 * MAP_NORESERVE mappings may also need pages and quota allocated
	 * if no reserve mapping overlaps.
*/
chg = vma_needs_reservation(h, vma, addr);
if (chg < 0)
return ERR_PTR(-VM_FAULT_OOM);
if (chg)
		if (hugetlb_get_quota(inode->i_mapping, chg))
return ERR_PTR(-VM_FAULT_SIGBUS);

spin_lock(&hugetlb_lock);
page = dequeue_huge_page_vma(h, vma, addr, avoid_reserve);
spin_unlock(&hugetlb_lock);

if (!page) {
page = alloc_buddy_huge_page(h, NUMA_NO_NODE);
if (!page) {
			hugetlb_put_quota(inode->i_mapping, chg);
return ERR_PTR(-VM_FAULT_SIGBUS);
}
}

	set_page_private(page, (unsigned long) mapping);

vma_commit_reservation(h, vma, addr);

return page;
}
