isoent_gen_joliet_identifier(struct archive_write *a, struct isoent *isoent,
struct idr *idr)
{
struct iso9660 *iso9660;
struct isoent *np;
unsigned char *p;
size_t l;
int r;
	int ffmax, parent_len;
static const struct archive_rb_tree_ops rb_ops = {
isoent_cmp_node_joliet, isoent_cmp_key_joliet
};

if (isoent->children.cnt == 0)
return (0);

iso9660 = a->format_data;
if (iso9660->opt.joliet == OPT_JOLIET_LONGNAME)
ffmax = 206;
else
ffmax = 128;

	r = idr_start(a, idr, isoent->children.cnt, ffmax, 6, 2, &rb_ops);
if (r < 0)
return (r);

parent_len = 1;
for (np = isoent; np->parent != np; np = np->parent)
parent_len += np->mb_len + 1;

for (np = isoent->children.first; np != NULL; np = np->chnext) {
unsigned char *dot;
int ext_off, noff, weight;
size_t lt;

		if ((int)(l = np->file->basename_utf16.length) > ffmax)
l = ffmax;

p = malloc((l+1)*2);
if (p == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory");
return (ARCHIVE_FATAL);
}
memcpy(p, np->file->basename_utf16.s, l);
p[l] = 0;
p[l+1] = 0;

np->identifier = (char *)p;
lt = l;
dot = p + l;
weight = 0;
while (lt > 0) {
if (!joliet_allowed_char(p[0], p[1]))
archive_be16enc(p, 0x005F); /* '_' */
else if (p[0] == 0 && p[1] == 0x2E) /* '.' */
dot = p;
p += 2;
lt -= 2;
}
ext_off = (int)(dot - (unsigned char *)np->identifier);
np->ext_off = ext_off;
np->ext_len = (int)l - ext_off;
np->id_len = (int)l;

/*
* Get a length of MBS of a full-pathname.
*/
		if ((int)np->file->basename_utf16.length > ffmax) {
if (archive_strncpy_l(&iso9660->mbs,
(const char *)np->identifier, l,
iso9660->sconv_from_utf16be) != 0 &&
errno == ENOMEM) {
archive_set_error(&a->archive, errno,
"No memory");
return (ARCHIVE_FATAL);
}
np->mb_len = (int)iso9660->mbs.length;
if (np->mb_len != (int)np->file->basename.length)
weight = np->mb_len;
} else
np->mb_len = (int)np->file->basename.length;

/* If a length of full-pathname is longer than 240 bytes,
* it violates Joliet extensions regulation. */
		if (parent_len + np->mb_len > 240) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"The regulation of Joliet extensions;"
" A length of a full-pathname of `%s' is "
"longer than 240 bytes, (p=%d, b=%d)",
archive_entry_pathname(np->file->entry),
(int)parent_len, (int)np->mb_len);
return (ARCHIVE_FATAL);
}

/* Make an offset of the number which is used to be set
* hexadecimal number to avoid duplicate identifier. */
		if ((int)l == ffmax)
noff = ext_off - 6;
		else if ((int)l == ffmax-2)
noff = ext_off - 4;
		else if ((int)l == ffmax-4)
noff = ext_off - 2;
else
noff = ext_off;
/* Register entry to the identifier resolver. */
idr_register(idr, np, weight, noff);
}

/* Resolve duplicate identifier with Joliet Volume. */
idr_resolve(idr, idr_set_num_beutf16);

return (ARCHIVE_OK);
}
