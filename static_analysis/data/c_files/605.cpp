checked_xcalloc (size_t num, size_t size)
{
    alloc_limit_assert ("checked_xcalloc", (num *size));
return xcalloc (num, size);
}
