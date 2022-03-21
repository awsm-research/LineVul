static int command_read(struct pci_dev *dev, int offset, u16 *value, void *data)
{
	int i;
	int ret;
	ret = xen_pcibk_read_config_word(dev, offset, value, data);
	if (!pci_is_enabled(dev))
		return ret;
	for (i = 0; i < PCI_ROM_RESOURCE; i++) {
		if (dev->resource[i].flags & IORESOURCE_IO)
			*value |= PCI_COMMAND_IO;
		if (dev->resource[i].flags & IORESOURCE_MEM)
			*value |= PCI_COMMAND_MEMORY;
}

return ret;
}
