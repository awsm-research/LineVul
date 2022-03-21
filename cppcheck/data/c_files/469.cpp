static void *__alloc_from_pool(size_t size, struct page **ret_page, gfp_t flags)
{
unsigned long val;
void *ptr = NULL;

if (!atomic_pool) {
WARN(1, "coherent pool not initialised!\n");
return NULL;
}

val = gen_pool_alloc(atomic_pool, size);
if (val) {
phys_addr_t phys = gen_pool_virt_to_phys(atomic_pool, val);

*ret_page = phys_to_page(phys);
ptr = (void *)val;
		if (flags & __GFP_ZERO)
			memset(ptr, 0, size);
}

return ptr;
}
