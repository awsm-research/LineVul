void xmlrpc_char_encode(char *outbuffer, const char *s1)
{
long unsigned int i;
unsigned char c;
char buf2[15];
mowgli_string_t *s = mowgli_string_create();
*buf2 = '\0';
*outbuffer = '\0';

if ((!(s1) || (*(s1) == '\0')))
{
return;
}

for (i = 0; s1[i] != '\0'; i++)
{
c = s1[i];
if (c > 127)
{
snprintf(buf2, sizeof buf2, "&#%d;", c);
s->append(s, buf2, strlen(buf2));
}
else if (c == '&')
{
s->append(s, "&amp;", 5);
}
else if (c == '<')
{
s->append(s, "&lt;", 4);
}
else if (c == '>')
{
s->append(s, "&gt;", 4);
}
else if (c == '"')
{
s->append(s, "&quot;", 6);
}
else
{
s->append_char(s, c);
}
}

	memcpy(outbuffer, s->str, XMLRPC_BUFSIZE);
}
