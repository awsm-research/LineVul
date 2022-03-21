static enum integrity_status evm_verify_hmac(struct dentry *dentry,
const char *xattr_name,
char *xattr_value,
size_t xattr_value_len,
struct integrity_iint_cache *iint)
{
struct evm_ima_xattr_data *xattr_data = NULL;
struct evm_ima_xattr_data calc;
enum integrity_status evm_status = INTEGRITY_PASS;
int rc, xattr_len;

if (iint && iint->evm_status == INTEGRITY_PASS)
return iint->evm_status;

/* if status is not PASS, try to check again - against -ENOMEM */

/* first need to know the sig type */
rc = vfs_getxattr_alloc(dentry, XATTR_NAME_EVM, (char **)&xattr_data, 0,
GFP_NOFS);
if (rc <= 0) {
evm_status = INTEGRITY_FAIL;
if (rc == -ENODATA) {
rc = evm_find_protected_xattrs(dentry);
if (rc > 0)
evm_status = INTEGRITY_NOLABEL;
else if (rc == 0)
evm_status = INTEGRITY_NOXATTRS; /* new file */
} else if (rc == -EOPNOTSUPP) {
evm_status = INTEGRITY_UNKNOWN;
}
goto out;
}

xattr_len = rc;

/* check value type */
switch (xattr_data->type) {
case EVM_XATTR_HMAC:
rc = evm_calc_hmac(dentry, xattr_name, xattr_value,
xattr_value_len, calc.digest);
if (rc)
break;
		rc = memcmp(xattr_data->digest, calc.digest,
sizeof(calc.digest));
if (rc)
rc = -EINVAL;
break;
case EVM_IMA_XATTR_DIGSIG:
rc = evm_calc_hash(dentry, xattr_name, xattr_value,
xattr_value_len, calc.digest);
if (rc)
break;
rc = integrity_digsig_verify(INTEGRITY_KEYRING_EVM,
(const char *)xattr_data, xattr_len,
calc.digest, sizeof(calc.digest));
if (!rc) {
/* Replace RSA with HMAC if not mounted readonly and
* not immutable
*/
if (!IS_RDONLY(d_backing_inode(dentry)) &&
!IS_IMMUTABLE(d_backing_inode(dentry)))
evm_update_evmxattr(dentry, xattr_name,
xattr_value,
xattr_value_len);
}
break;
default:
rc = -EINVAL;
break;
}

if (rc)
evm_status = (rc == -ENODATA) ?
INTEGRITY_NOXATTRS : INTEGRITY_FAIL;
out:
if (iint)
iint->evm_status = evm_status;
kfree(xattr_data);
return evm_status;
}
