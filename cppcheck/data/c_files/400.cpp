_zip_cdir_new(int nentry, struct zip_error *error)
{
struct zip_cdir *cd;

if ((cd=(struct zip_cdir *)malloc(sizeof(*cd))) == NULL) {
_zip_error_set(error, ZIP_ER_MEMORY, 0);
return NULL;
}

    if ((cd->entry=(struct zip_dirent *)malloc(sizeof(*(cd->entry))*nentry))
== NULL) {
_zip_error_set(error, ZIP_ER_MEMORY, 0);
free(cd);
return NULL;
}

/* entries must be initialized by caller */

cd->nentry = nentry;
cd->size = cd->offset = 0;
cd->comment = NULL;
cd->comment_len = 0;

return cd;
}
