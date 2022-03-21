static char *rfc2047_decode_word(const char *s, size_t len, enum ContentEncoding enc)
{
const char *it = s;
const char *end = s + len;

if (enc == ENCQUOTEDPRINTABLE)
{
struct Buffer buf = { 0 };
for (; it < end; ++it)
{
if (*it == '_')
{
mutt_buffer_addch(&buf, ' ');
}
else if ((*it == '=') && (!(it[1] & ~127) && hexval(it[1]) != -1) &&
(!(it[2] & ~127) && hexval(it[2]) != -1))
{
mutt_buffer_addch(&buf, (hexval(it[1]) << 4) | hexval(it[2]));
it += 2;
}
else
{
mutt_buffer_addch(&buf, *it);
}
}
mutt_buffer_addch(&buf, '\0');
return buf.data;
}
else if (enc == ENCBASE64)
{
    char *out = mutt_mem_malloc(3 * len / 4 + 1);
    int dlen = mutt_b64_decode(out, it);
if (dlen == -1)
{
FREE(&out);
return NULL;
}
out[dlen] = '\0';
return out;
}

assert(0); /* The enc parameter has an invalid value */
return NULL;
}
