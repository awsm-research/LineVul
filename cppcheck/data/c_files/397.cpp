static struct nfs4_opendata *nfs4_opendata_alloc(struct path *path,
		struct nfs4_state_owner *sp, int flags,
const struct iattr *attrs)
{
struct dentry *parent = dget_parent(path->dentry);
struct inode *dir = parent->d_inode;
struct nfs_server *server = NFS_SERVER(dir);
struct nfs4_opendata *p;

p = kzalloc(sizeof(*p), GFP_KERNEL);
if (p == NULL)
goto err;
p->o_arg.seqid = nfs_alloc_seqid(&sp->so_seqid);
if (p->o_arg.seqid == NULL)
goto err_free;
p->path.mnt = mntget(path->mnt);
p->path.dentry = dget(path->dentry);
p->dir = parent;
p->owner = sp;
atomic_inc(&sp->so_count);
p->o_arg.fh = NFS_FH(dir);
	p->o_arg.open_flags = flags,
p->o_arg.clientid = server->nfs_client->cl_clientid;
p->o_arg.id = sp->so_owner_id.id;
p->o_arg.name = &p->path.dentry->d_name;
p->o_arg.server = server;
p->o_arg.bitmask = server->attr_bitmask;
p->o_arg.claim = NFS4_OPEN_CLAIM_NULL;
if (flags & O_EXCL) {
u32 *s = (u32 *) p->o_arg.u.verifier.data;
s[0] = jiffies;
s[1] = current->pid;
} else if (flags & O_CREAT) {
p->o_arg.u.attrs = &p->attrs;
memcpy(&p->attrs, attrs, sizeof(p->attrs));
}
p->c_arg.fh = &p->o_res.fh;
p->c_arg.stateid = &p->o_res.stateid;
p->c_arg.seqid = p->o_arg.seqid;
nfs4_init_opendata_res(p);
kref_init(&p->kref);
return p;
err_free:
kfree(p);
err:
dput(parent);
return NULL;
}
