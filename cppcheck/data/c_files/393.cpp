static netdev_tx_t hns_nic_net_xmit(struct sk_buff *skb,
struct net_device *ndev)
{
struct hns_nic_priv *priv = netdev_priv(ndev);
	int ret;

assert(skb->queue_mapping < ndev->ae_handle->q_num);
	ret = hns_nic_net_xmit_hw(ndev, skb,
				  &tx_ring_data(priv, skb->queue_mapping));
	if (ret == NETDEV_TX_OK) {
		netif_trans_update(ndev);
		ndev->stats.tx_bytes += skb->len;
		ndev->stats.tx_packets++;
	}
	return (netdev_tx_t)ret;
}
