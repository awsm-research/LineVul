static void svc_rdma_destroy_maps(struct svcxprt_rdma *xprt)
{
	while (!list_empty(&xprt->sc_maps)) {
		struct svc_rdma_req_map *map;
		map = list_first_entry(&xprt->sc_maps,
				       struct svc_rdma_req_map, free);
		list_del(&map->free);
		kfree(map);
	}
}
