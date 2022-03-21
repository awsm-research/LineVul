struct sk_buff **udp_gro_receive(struct sk_buff **head, struct sk_buff *skb,
struct udphdr *uh)
{
struct udp_offload_priv *uo_priv;
struct sk_buff *p, **pp = NULL;
struct udphdr *uh2;
unsigned int off = skb_gro_offset(skb);
int flush = 1;

	if (NAPI_GRO_CB(skb)->udp_mark ||
(skb->ip_summed != CHECKSUM_PARTIAL &&
NAPI_GRO_CB(skb)->csum_cnt == 0 &&
!NAPI_GRO_CB(skb)->csum_valid))
goto out;

	/* mark that this skb passed once through the udp gro layer *
	NAPI_GRO_CB(skb)->udp_mark = 1;

rcu_read_lock();
uo_priv = rcu_dereference(udp_offload_base);
for (; uo_priv != NULL; uo_priv = rcu_dereference(uo_priv->next)) {
if (net_eq(read_pnet(&uo_priv->net), dev_net(skb->dev)) &&
uo_priv->offload->port == uh->dest &&
uo_priv->offload->callbacks.gro_receive)
goto unflush;
}
goto out_unlock;

unflush:
flush = 0;

for (p = *head; p; p = p->next) {
if (!NAPI_GRO_CB(p)->same_flow)
continue;

uh2 = (struct udphdr   *)(p->data + off);

/* Match ports and either checksums are either both zero
* or nonzero.
*/
if ((*(u32 *)&uh->source != *(u32 *)&uh2->source) ||
(!uh->check ^ !uh2->check)) {
NAPI_GRO_CB(p)->same_flow = 0;
continue;
}
}

skb_gro_pull(skb, sizeof(struct udphdr)); /* pull encapsulating udp header */
skb_gro_postpull_rcsum(skb, uh, sizeof(struct udphdr));
NAPI_GRO_CB(skb)->proto = uo_priv->offload->ipproto;
pp = uo_priv->offload->callbacks.gro_receive(head, skb,
uo_priv->offload);

out_unlock:
rcu_read_unlock();
out:
NAPI_GRO_CB(skb)->flush |= flush;
return pp;
}
