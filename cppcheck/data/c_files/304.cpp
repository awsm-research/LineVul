static int burl_normalize_2F_to_slash_fix (buffer *b, int qs, int i)
{
char * const s = b->ptr;
const int blen = (int)buffer_string_length(b);
const int used = qs < 0 ? blen : qs;
int j = i;
for (; i < used; ++i, ++j) {
s[j] = s[i];
if (s[i] == '%' && s[i+1] == '2' && s[i+2] == 'F') {
s[j] = '/';
i+=2;
}
}
if (qs >= 0) {
        memmove(s+j, s+qs, blen - qs);
        j += blen - qs;
}
buffer_string_set_length(b, j);
return qs;
}
