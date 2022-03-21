void *arm_dma_alloc(struct device *dev, size_t size, dma_addr_t *handle,
gfp_t gfp, struct dma_attrs *attrs)
{
	pgprot_t prot = __get_dma_pgprot(attrs, pgprot_kernel);
void *memory;

if (dma_alloc_from_coherent(dev, size, handle, &memory))
return memory;

return __dma_alloc(dev, size, handle, gfp, prot, false,
__builtin_return_address(0));
}
