pvscsi_ring_init_data(PVSCSIRingInfo *m, PVSCSICmdDescSetupRings *ri)
{
int i;
uint32_t txr_len_log2, rxr_len_log2;
uint32_t req_ring_size, cmp_ring_size;
m->rs_pa = ri->ringsStatePPN << VMW_PAGE_SHIFT;

    if ((ri->reqRingNumPages > PVSCSI_SETUP_RINGS_MAX_NUM_PAGES)
        || (ri->cmpRingNumPages > PVSCSI_SETUP_RINGS_MAX_NUM_PAGES)) {
        return -1;
    }
req_ring_size = ri->reqRingNumPages * PVSCSI_MAX_NUM_REQ_ENTRIES_PER_PAGE;
cmp_ring_size = ri->cmpRingNumPages * PVSCSI_MAX_NUM_CMP_ENTRIES_PER_PAGE;
txr_len_log2 = pvscsi_log2(req_ring_size - 1);
}
