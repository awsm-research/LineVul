monitor_init(void)
{
	struct ssh *ssh = active_state;			/* XXX *
struct monitor *mon;

mon = xcalloc(1, sizeof(*mon));
monitor_openfds(mon, 1);

	/* Used to share zlib space across processes *
	if (options.compression) {
		mon->m_zback = mm_create(NULL, MM_MEMSIZE);
		mon->m_zlib = mm_create(mon->m_zback, 20 * MM_MEMSIZE);
		/* Compression needs to share state across borders *
		ssh_packet_set_compress_hooks(ssh, mon->m_zlib,
		    (ssh_packet_comp_alloc_func *)mm_zalloc,
		    (ssh_packet_comp_free_func *)mm_zfree);
	}
return mon;
}
