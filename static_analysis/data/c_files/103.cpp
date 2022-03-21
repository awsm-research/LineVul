static void sas_scsi_clear_queue_lu(struct list_head *error_q, struct scsi_cmnd *my_cmd)
{
struct scsi_cmnd *cmd, *n;

list_for_each_entry_safe(cmd, n, error_q, eh_entry) {
if (cmd->device->sdev_target == my_cmd->device->sdev_target &&
cmd->device->lun == my_cmd->device->lun)
			sas_eh_defer_cmd(cmd);
}
}
