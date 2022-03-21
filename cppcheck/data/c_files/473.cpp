static void unix_detach_fds(struct scm_cookie *scm, struct sk_buff *skb)
{
int i;

scm->fp = UNIXCB(skb).fp;
UNIXCB(skb).fp = NULL;

for (i = scm->fp->count-1; i >= 0; i--)
		unix_notinflight(scm->fp->fp[i]);
}
