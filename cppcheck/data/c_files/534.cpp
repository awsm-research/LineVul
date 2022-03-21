int ceph_set_acl(struct inode *inode, struct posix_acl *acl, int type)
{
int ret = 0, size = 0;
const char *name = NULL;
char *value = NULL;
struct iattr newattrs;
umode_t new_mode = inode->i_mode, old_mode = inode->i_mode;

switch (type) {
case ACL_TYPE_ACCESS:
name = XATTR_NAME_POSIX_ACL_ACCESS;
if (acl) {
			ret = posix_acl_equiv_mode(acl, &new_mode);
			if (ret < 0)
goto out;
			if (ret == 0)
				acl = NULL;
}
break;
case ACL_TYPE_DEFAULT:
if (!S_ISDIR(inode->i_mode)) {
ret = acl ? -EINVAL : 0;
goto out;
}
name = XATTR_NAME_POSIX_ACL_DEFAULT;
break;
default:
ret = -EINVAL;
goto out;
}

if (acl) {
size = posix_acl_xattr_size(acl->a_count);
value = kmalloc(size, GFP_NOFS);
if (!value) {
ret = -ENOMEM;
goto out;
}

ret = posix_acl_to_xattr(&init_user_ns, acl, value, size);
if (ret < 0)
goto out_free;
}

if (new_mode != old_mode) {
newattrs.ia_mode = new_mode;
newattrs.ia_valid = ATTR_MODE;
ret = __ceph_setattr(inode, &newattrs);
if (ret)
goto out_free;
}

ret = __ceph_setxattr(inode, name, value, size, 0);
if (ret) {
if (new_mode != old_mode) {
newattrs.ia_mode = old_mode;
newattrs.ia_valid = ATTR_MODE;
__ceph_setattr(inode, &newattrs);
}
goto out_free;
}

ceph_set_cached_acl(inode, type, acl);

out_free:
kfree(value);
out:
return ret;
}
