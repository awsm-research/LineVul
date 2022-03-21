void svc_rdma_send_error(struct svcxprt_rdma *xprt, struct rpcrdma_msg *rmsgp,
			 int status)
{
	struct ib_send_wr err_wr;
	struct page *p;
	struct svc_rdma_op_ctxt *ctxt;
	enum rpcrdma_errcode err;
	__be32 *va;
	int length;
	int ret;
	ret = svc_rdma_repost_recv(xprt, GFP_KERNEL);
	if (ret)
		return;
	p = alloc_page(GFP_KERNEL);
	if (!p)
		return;
	va = page_address(p);
	/* XDR encode an error reply *
	err = ERR_CHUNK;
	if (status == -EPROTONOSUPPORT)
		err = ERR_VERS;
	length = svc_rdma_xdr_encode_error(xprt, rmsgp, err, va);
	ctxt = svc_rdma_get_context(xprt);
	ctxt->direction = DMA_TO_DEVICE;
	ctxt->count = 1;
	ctxt->pages[0] = p;
	/* Prepare SGE for local address *
	ctxt->sge[0].lkey = xprt->sc_pd->local_dma_lkey;
	ctxt->sge[0].length = length;
	ctxt->sge[0].addr = ib_dma_map_page(xprt->sc_cm_id->device,
					    p, 0, length, DMA_TO_DEVICE);
	if (ib_dma_mapping_error(xprt->sc_cm_id->device, ctxt->sge[0].addr)) {
		dprintk("svcrdma: Error mapping buffer for protocol error\n");
		svc_rdma_put_context(ctxt, 1);
		return;
	}
	svc_rdma_count_mappings(xprt, ctxt);
	/* Prepare SEND WR *
	memset(&err_wr, 0, sizeof(err_wr));
	ctxt->cqe.done = svc_rdma_wc_send;
	err_wr.wr_cqe = &ctxt->cqe;
	err_wr.sg_list = ctxt->sge;
	err_wr.num_sge = 1;
	err_wr.opcode = IB_WR_SEND;
	err_wr.send_flags = IB_SEND_SIGNALED;
	/* Post It *
	ret = svc_rdma_send(xprt, &err_wr);
	if (ret) {
		dprintk("svcrdma: Error %d posting send for protocol error\n",
			ret);
		svc_rdma_unmap_dma(ctxt);
		svc_rdma_put_context(ctxt, 1);
	}
}
