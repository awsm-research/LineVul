static ssize_t o2nm_node_ipv4_address_store(struct config_item *item,
const char *page,
size_t count)
{
struct o2nm_node *node = to_o2nm_node(item);
	struct o2nm_cluster *cluster = to_o2nm_cluster_from_node(node);
int ret, i;
struct rb_node **p, *parent;
unsigned int octets[4];
__be32 ipv4_addr = 0;

ret = sscanf(page, "%3u.%3u.%3u.%3u", &octets[3], &octets[2],
&octets[1], &octets[0]);
if (ret != 4)
return -EINVAL;

for (i = 0; i < ARRAY_SIZE(octets); i++) {
if (octets[i] > 255)
return -ERANGE;
be32_add_cpu(&ipv4_addr, octets[i] << (i * 8));
}

ret = 0;
write_lock(&cluster->cl_nodes_lock);
if (o2nm_node_ip_tree_lookup(cluster, ipv4_addr, &p, &parent))
ret = -EEXIST;
else if (test_and_set_bit(O2NM_NODE_ATTR_ADDRESS,
&node->nd_set_attributes))
ret = -EBUSY;
else {
rb_link_node(&node->nd_ip_node, parent, p);
rb_insert_color(&node->nd_ip_node, &cluster->cl_node_ip_tree);
}
write_unlock(&cluster->cl_nodes_lock);
if (ret)
return ret;

memcpy(&node->nd_ipv4_address, &ipv4_addr, sizeof(ipv4_addr));

return count;
}
