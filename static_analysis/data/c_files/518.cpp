char *path_name(const struct name_path *path, const char *name)
{
const struct name_path *p;
char *n, *m;
int nlen = strlen(name);
int len = nlen + 1;

for (p = path; p; p = p->up) {
if (p->elem_len)
len += p->elem_len + 1;
}
n = xmalloc(len);
m = n + len - (nlen + 1);
	strcpy(m, name);
for (p = path; p; p = p->up) {
if (p->elem_len) {
m -= p->elem_len + 1;
memcpy(m, p->elem, p->elem_len);
m[p->elem_len] = '/';
}
}
return n;
}
