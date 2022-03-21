static void ifb_setup(struct net_device *dev)
{
/* Initialize the device structure. */
dev->destructor = free_netdev;
dev->netdev_ops = &ifb_netdev_ops;

/* Fill in device structure with ethernet-generic values. */
ether_setup(dev);
dev->tx_queue_len = TX_Q_LIMIT;

dev->features |= IFB_FEATURES;
dev->vlan_features |= IFB_FEATURES;

dev->flags |= IFF_NOARP;
dev->flags &= ~IFF_MULTICAST;
	dev->priv_flags &= ~IFF_XMIT_DST_RELEASE;
random_ether_addr(dev->dev_addr);
}
