static int hns_xgmac_get_sset_count(int stringset)
{
	if (stringset == ETH_SS_STATS)
return ARRAY_SIZE(g_xgmac_stats_string);

return 0;
}
