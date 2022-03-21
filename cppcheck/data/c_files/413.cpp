TEE_Result tee_mmu_check_access_rights(const struct user_ta_ctx *utc,
uint32_t flags, uaddr_t uaddr,
size_t len)
{
uaddr_t a;
size_t addr_incr = MIN(CORE_MMU_USER_CODE_SIZE,
CORE_MMU_USER_PARAM_SIZE);

	if (ADD_OVERFLOW(uaddr, len, &a))
return TEE_ERROR_ACCESS_DENIED;

if ((flags & TEE_MEMORY_ACCESS_NONSECURE) &&
(flags & TEE_MEMORY_ACCESS_SECURE))
return TEE_ERROR_ACCESS_DENIED;

/*
* Rely on TA private memory test to check if address range is private
* to TA or not.
*/
if (!(flags & TEE_MEMORY_ACCESS_ANY_OWNER) &&
!tee_mmu_is_vbuf_inside_ta_private(utc, (void *)uaddr, len))
return TEE_ERROR_ACCESS_DENIED;

	for (a = uaddr; a < (uaddr + len); a += addr_incr) {
uint32_t attr;
TEE_Result res;

res = tee_mmu_user_va2pa_attr(utc, (void *)a, NULL, &attr);
if (res != TEE_SUCCESS)
return res;

if ((flags & TEE_MEMORY_ACCESS_NONSECURE) &&
(attr & TEE_MATTR_SECURE))
return TEE_ERROR_ACCESS_DENIED;

if ((flags & TEE_MEMORY_ACCESS_SECURE) &&
!(attr & TEE_MATTR_SECURE))
return TEE_ERROR_ACCESS_DENIED;

if ((flags & TEE_MEMORY_ACCESS_WRITE) && !(attr & TEE_MATTR_UW))
return TEE_ERROR_ACCESS_DENIED;
if ((flags & TEE_MEMORY_ACCESS_READ) && !(attr & TEE_MATTR_UR))
return TEE_ERROR_ACCESS_DENIED;
}

return TEE_SUCCESS;
}
