int svc_rdma_map_xdr(struct svcxprt_rdma *xprt,
		     struct xdr_buf *xdr,
		     struct svc_rdma_req_map *vec,
		     bool write_chunk_present)
{
	int sge_no;
	u32 sge_bytes;
	u32 page_bytes;
	u32 page_off;
	int page_no;
	if (xdr->len !=
	    (xdr->head[0].iov_len + xdr->page_len + xdr->tail[0].iov_len)) {
		pr_err("svcrdma: %s: XDR buffer length error\n", __func__);
		return -EIO;
	}

	/* Skip the first sge, this is for the RPCRDMA header *
	sge_no = 1;

	/* Head SGE *
	vec->sge[sge_no].iov_base = xdr->head[0].iov_base;
	vec->sge[sge_no].iov_len = xdr->head[0].iov_len;
	sge_no++;
	/* pages SGE *
	page_no = 0;
	page_bytes = xdr->page_len;
	page_off = xdr->page_base;
	while (page_bytes) {
		vec->sge[sge_no].iov_base =
			page_address(xdr->pages[page_no]) + page_off;
		sge_bytes = min_t(u32, page_bytes, (PAGE_SIZE - page_off));
		page_bytes -= sge_bytes;
		vec->sge[sge_no].iov_len = sge_bytes;
		sge_no++;
		page_no++;
		page_off = 0; /* reset for next time through loop *
}

	/* Tail SGE *
	if (xdr->tail[0].iov_len) {
		unsigned char *base = xdr->tail[0].iov_base;
		size_t len = xdr->tail[0].iov_len;
		u32 xdr_pad = xdr_padsize(xdr->page_len);

		if (write_chunk_present && xdr_pad) {
			base += xdr_pad;
			len -= xdr_pad;
		}

		if (len) {
			vec->sge[sge_no].iov_base = base;
			vec->sge[sge_no].iov_len = len;
			sge_no++;
}
	}

	dprintk("svcrdma: %s: sge_no %d page_no %d "
		"page_base %u page_len %u head_len %zu tail_len %zu\n",
		__func__, sge_no, page_no, xdr->page_base, xdr->page_len,
		xdr->head[0].iov_len, xdr->tail[0].iov_len);

	vec->count = sge_no;
	return 0;
}
