vhost_scsi_make_tpg(struct se_wwn *wwn,
struct config_group *group,
const char *name)
{
struct vhost_scsi_tport *tport = container_of(wwn,
struct vhost_scsi_tport, tport_wwn);

struct vhost_scsi_tpg *tpg;
	unsigned long tpgt;
int ret;

if (strstr(name, "tpgt_") != name)
return ERR_PTR(-EINVAL);
	if (kstrtoul(name + 5, 10, &tpgt) || tpgt > UINT_MAX)
return ERR_PTR(-EINVAL);

tpg = kzalloc(sizeof(struct vhost_scsi_tpg), GFP_KERNEL);
if (!tpg) {
pr_err("Unable to allocate struct vhost_scsi_tpg");
return ERR_PTR(-ENOMEM);
}
mutex_init(&tpg->tv_tpg_mutex);
INIT_LIST_HEAD(&tpg->tv_tpg_list);
tpg->tport = tport;
tpg->tport_tpgt = tpgt;

ret = core_tpg_register(&vhost_scsi_fabric_configfs->tf_ops, wwn,
&tpg->se_tpg, tpg, TRANSPORT_TPG_TYPE_NORMAL);
if (ret < 0) {
kfree(tpg);
return NULL;
}
mutex_lock(&vhost_scsi_mutex);
list_add_tail(&tpg->tv_tpg_list, &vhost_scsi_list);
mutex_unlock(&vhost_scsi_mutex);

return &tpg->se_tpg;
}
