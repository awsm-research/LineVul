static int mptsas_process_scsi_io_request(MPTSASState *s,
MPIMsgSCSIIORequest *scsi_io,
hwaddr addr)
{
MPTSASRequest *req;
MPIMsgSCSIIOReply reply;
SCSIDevice *sdev;
int status;

mptsas_fix_scsi_io_endianness(scsi_io);

trace_mptsas_process_scsi_io_request(s, scsi_io->Bus, scsi_io->TargetID,
scsi_io->LUN[1], scsi_io->DataLength);

status = mptsas_scsi_device_find(s, scsi_io->Bus, scsi_io->TargetID,
scsi_io->LUN, &sdev);
if (status) {
goto bad;
}

    req = g_new(MPTSASRequest, 1);
QTAILQ_INSERT_TAIL(&s->pending, req, next);
req->scsi_io = *scsi_io;
req->dev = s;

status = mptsas_build_sgl(s, req, addr);
if (status) {
goto free_bad;
}

if (req->qsg.size < scsi_io->DataLength) {
trace_mptsas_sgl_overflow(s, scsi_io->MsgContext, scsi_io->DataLength,
req->qsg.size);
status = MPI_IOCSTATUS_INVALID_SGL;
goto free_bad;
}

req->sreq = scsi_req_new(sdev, scsi_io->MsgContext,
scsi_io->LUN[1], scsi_io->CDB, req);

if (req->sreq->cmd.xfer > scsi_io->DataLength) {
goto overrun;
}
switch (scsi_io->Control & MPI_SCSIIO_CONTROL_DATADIRECTION_MASK) {
case MPI_SCSIIO_CONTROL_NODATATRANSFER:
if (req->sreq->cmd.mode != SCSI_XFER_NONE) {
goto overrun;
}
break;

case MPI_SCSIIO_CONTROL_WRITE:
if (req->sreq->cmd.mode != SCSI_XFER_TO_DEV) {
goto overrun;
}
break;

case MPI_SCSIIO_CONTROL_READ:
if (req->sreq->cmd.mode != SCSI_XFER_FROM_DEV) {
goto overrun;
}
break;
}

if (scsi_req_enqueue(req->sreq)) {
scsi_req_continue(req->sreq);
}
return 0;

overrun:
trace_mptsas_scsi_overflow(s, scsi_io->MsgContext, req->sreq->cmd.xfer,
scsi_io->DataLength);
status = MPI_IOCSTATUS_SCSI_DATA_OVERRUN;
free_bad:
mptsas_free_request(req);
bad:
memset(&reply, 0, sizeof(reply));
reply.TargetID          = scsi_io->TargetID;
reply.Bus               = scsi_io->Bus;
reply.MsgLength         = sizeof(reply) / 4;
reply.Function          = scsi_io->Function;
reply.CDBLength         = scsi_io->CDBLength;
reply.SenseBufferLength = scsi_io->SenseBufferLength;
reply.MsgContext        = scsi_io->MsgContext;
reply.SCSIState         = MPI_SCSI_STATE_NO_SCSI_STATUS;
reply.IOCStatus         = status;

mptsas_fix_scsi_io_reply_endianness(&reply);
mptsas_reply(s, (MPIDefaultReply *)&reply);

return 0;
}
