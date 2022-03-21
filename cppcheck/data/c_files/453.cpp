dns_resolver_match(const struct key *key,
		   const struct key_match_data *match_data)
{
int slen, dlen, ret = 0;
const char *src = key->description, *dsp = match_data->raw_data;

kenter("%s,%s", src, dsp);

if (!src || !dsp)
goto no_match;

if (strcasecmp(src, dsp) == 0)
goto matched;

slen = strlen(src);
dlen = strlen(dsp);
if (slen <= 0 || dlen <= 0)
goto no_match;
if (src[slen - 1] == '.')
slen--;
if (dsp[dlen - 1] == '.')
dlen--;
if (slen != dlen || strncasecmp(src, dsp, slen) != 0)
goto no_match;

matched:
ret = 1;
no_match:
kleave(" = %d", ret);
return ret;
}
