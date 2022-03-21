parse_field(netdissect_options *ndo, const char **pptr, int *len)
{
const char *s;

    if (*len <= 0 || !pptr || !*pptr)
	return NULL;
    if (*pptr > (const char *) ndo->ndo_snapend)
	return NULL;
s = *pptr;
    while (*pptr <= (const char *) ndo->ndo_snapend && *len >= 0 && **pptr) {
(*pptr)++;
(*len)--;
}
(*pptr)++;
(*len)--;
    if (*len < 0 || *pptr > (const char *) ndo->ndo_snapend)
	return NULL;
return s;
}
