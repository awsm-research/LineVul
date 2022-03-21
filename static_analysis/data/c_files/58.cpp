struct import_t* MACH0_(get_imports)(struct MACH0_(obj_t)* bin) {
struct import_t *imports;
int i, j, idx, stridx;
const char *symstr;

	if (!bin->symtab || !bin->symstr || !bin->sects || !bin->indirectsyms)
return NULL;
if (bin->dysymtab.nundefsym < 1 || bin->dysymtab.nundefsym > 0xfffff) {
return NULL;
}
if (!(imports = malloc ((bin->dysymtab.nundefsym + 1) * sizeof (struct import_t)))) {
return NULL;
}
for (i = j = 0; i < bin->dysymtab.nundefsym; i++) {
idx = bin->dysymtab.iundefsym + i;
if (idx < 0 || idx >= bin->nsymtab) {
bprintf ("WARNING: Imports index out of bounds. Ignoring relocs\n");
free (imports);
return NULL;
}
stridx = bin->symtab[idx].n_strx;
if (stridx >= 0 && stridx < bin->symstrlen) {
symstr = (char *)bin->symstr + stridx;
} else {
symstr = "";
}
if (!*symstr) {
continue;
}
{
int i = 0;
int len = 0;
char *symstr_dup = NULL;
len = bin->symstrlen - stridx;
imports[j].name[0] = 0;
if (len > 0) {
for (i = 0; i < len; i++) {
if ((unsigned char)symstr[i] == 0xff || !symstr[i]) {
len = i;
break;
}
}
symstr_dup = r_str_ndup (symstr, len);
if (symstr_dup) {
r_str_ncpy (imports[j].name, symstr_dup, R_BIN_MACH0_STRING_LENGTH);
r_str_filter (imports[j].name, - 1);
imports[j].name[R_BIN_MACH0_STRING_LENGTH - 2] = 0;
free (symstr_dup);
}
}
}
imports[j].ord = i;
imports[j++].last = 0;
}
imports[j].last = 1;

if (!bin->imports_by_ord_size) {
if (j > 0) {
bin->imports_by_ord_size = j;
bin->imports_by_ord = (RBinImport**)calloc (j, sizeof (RBinImport*));
} else {
bin->imports_by_ord_size = 0;
bin->imports_by_ord = NULL;
}
}

return imports;
}
