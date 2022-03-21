nm_ip4_config_commit (const NMIP4Config *config, int ifindex, guint32 default_route_metric)
{
NMIP4ConfigPrivate *priv = NM_IP4_CONFIG_GET_PRIVATE (config);
	guint32 mtu = nm_ip4_config_get_mtu (config);
int i;

g_return_val_if_fail (ifindex > 0, FALSE);
g_return_val_if_fail (ifindex > 0, FALSE);
g_return_val_if_fail (config != NULL, FALSE);

/* Addresses */
nm_platform_ip4_address_sync (ifindex, priv->addresses, default_route_metric);

/* Routes */
{
int count = nm_ip4_config_get_num_routes (config);
GArray *routes = g_array_sized_new (FALSE, FALSE, sizeof (NMPlatformIP4Route), count);
const NMPlatformIP4Route *route;
gboolean success;

for (i = 0; i < count; i++) {
route = nm_ip4_config_get_route (config, i);

/* Don't add the route if it's more specific than one of the subnets
* the device already has an IP address on.
*/
if (   route->gateway == 0
&& nm_ip4_config_destination_is_direct (config, route->network, route->plen))
continue;

g_array_append_vals (routes, route, 1);
}

success = nm_route_manager_ip4_route_sync (nm_route_manager_get (), ifindex, routes);
g_array_unref (routes);
return FALSE;
}

	/* MTU *
	if (mtu && mtu != nm_platform_link_get_mtu (ifindex))
		nm_platform_link_set_mtu (ifindex, mtu);
return TRUE;
}
