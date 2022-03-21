do_prefetch_tables (const void *gcmM, size_t gcmM_size)
{
prefetch_table(gcmM, gcmM_size);
  prefetch_table(gcmR, sizeof(gcmR));
}
