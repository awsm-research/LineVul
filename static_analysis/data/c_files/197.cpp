static int iwch_l2t_send(struct t3cdev *tdev, struct sk_buff *skb, struct l2t_entry *l2e)
{
int	error = 0;
struct cxio_rdev *rdev;

rdev = (struct cxio_rdev *)tdev->ulp;
if (cxio_fatal_error(rdev)) {
kfree_skb(skb);
return -EIO;
}
error = l2t_send(tdev, skb, l2e);
if (error < 0)
kfree_skb(skb);
	return error;
}
