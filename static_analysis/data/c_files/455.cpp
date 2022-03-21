bgp_nlri_parse_vpnv4 (struct peer *peer, struct attr *attr,
struct bgp_nlri *packet)
{
u_char *pnt;
u_char *lim;
struct prefix p;
int psize;
int prefixlen;
u_int16_t type;
struct rd_as rd_as;
struct rd_ip rd_ip;
struct prefix_rd prd;
u_char *tagpnt;

/* Check peer status. */
if (peer->status != Established)
return 0;

/* Make prefix_rd */
prd.family = AF_UNSPEC;
prd.prefixlen = 64;

pnt = packet->nlri;
lim = pnt + packet->length;

for (; pnt < lim; pnt += psize)
{
/* Clear prefix structure. */


/* Fetch prefix length. */
prefixlen = *pnt++;
      p.family = AF_INET;
psize = PSIZE (prefixlen);
      if (prefixlen < 88)
	{
	  zlog_err ("prefix length is less than 88: %d", prefixlen);
	  return -1;
	}
/* Copyr label to prefix. */
      tagpnt = pnt;;

/* Copy routing distinguisher to rd. */
memcpy (&prd.val, pnt + 3, 8);
else if (type == RD_TYPE_IP)
zlog_info ("prefix %ld:%s:%ld:%s/%d", label, inet_ntoa (rd_ip.ip),
rd_ip.val, inet_ntoa (p.u.prefix4), p.prefixlen);
#endif /* 0 */

if (pnt + psize > lim)
return -1;

if (attr)
bgp_update (peer, &p, attr, AFI_IP, SAFI_MPLS_VPN,
ZEBRA_ROUTE_BGP, BGP_ROUTE_NORMAL, &prd, tagpnt, 0);
else
return -1;
}

      p.prefixlen = prefixlen - 88;
      memcpy (&p.u.prefix, pnt + 11, psize - 11);

#if 0
if (type == RD_TYPE_AS)
}
