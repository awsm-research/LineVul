void* sspi_SecureHandleGetLowerPointer(SecHandle* handle)
{
void* pointer;

	if (!handle)
return NULL;

pointer = (void*) ~((size_t) handle->dwLower);

return pointer;
}
