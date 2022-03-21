static size_t safecat(char *buffer, size_t bufsize, size_t pos,
   PNG_CONST char *cat)
{
while (pos < bufsize && cat != NULL && *cat != 0)
buffer[pos++] = *cat++;

if (pos >= bufsize)
pos = bufsize-1;

buffer[pos] = 0;
return pos;
}
