txid_snapshot_recv(PG_FUNCTION_ARGS)
{
StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);
TxidSnapshot *snap;
txid		last = 0;
int			nxip;
int			i;
	int			avail;
	int			expect;
txid		xmin,
xmax;

	/*
	 * load nxip and check for nonsense.
	 *
	 * (nxip > avail) check is against int overflows in 'expect'.
	 *
nxip = pq_getmsgint(buf, 4);
	avail = buf->len - buf->cursor;
	expect = 8 + 8 + nxip * 8;
	if (nxip < 0 || nxip > avail || expect > avail)
goto bad_format;

xmin = pq_getmsgint64(buf);
xmax = pq_getmsgint64(buf);
if (xmin == 0 || xmax == 0 || xmin > xmax || xmax > MAX_TXID)
goto bad_format;

snap = palloc(TXID_SNAPSHOT_SIZE(nxip));
snap->xmin = xmin;
snap->xmax = xmax;
snap->nxip = nxip;
SET_VARSIZE(snap, TXID_SNAPSHOT_SIZE(nxip));

for (i = 0; i < nxip; i++)
{
txid		cur = pq_getmsgint64(buf);

if (cur <= last || cur < xmin || cur >= xmax)
goto bad_format;
snap->xip[i] = cur;
last = cur;
}
PG_RETURN_POINTER(snap);

bad_format:
elog(ERROR, "invalid snapshot data");
return (Datum) NULL;
}
