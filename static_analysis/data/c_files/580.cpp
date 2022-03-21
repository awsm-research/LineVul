PHP_METHOD(Phar, extractTo)
{
char *error = NULL;
php_stream *fp;
php_stream_statbuf ssb;
phar_entry_info *entry;
char *pathto, *filename;
size_t pathto_len, filename_len;
int ret, i;
int nelems;
zval *zval_files = NULL;
zend_bool overwrite = 0;

PHAR_ARCHIVE_OBJECT();

       if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z!b", &pathto, &pathto_len, &zval_files, &overwrite) == FAILURE) {
return;
}

fp = php_stream_open_wrapper(phar_obj->archive->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK, NULL);

if (!fp) {
zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
"Invalid argument, %s cannot be found", phar_obj->archive->fname);
return;
}

php_stream_close(fp);

if (pathto_len < 1) {
zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
"Invalid argument, extraction path must be non-zero length");
return;
}

if (pathto_len >= MAXPATHLEN) {
char *tmp = estrndup(pathto, 50);
/* truncate for error message */
zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Cannot extract to \"%s...\", destination directory is too long for filesystem", tmp);
efree(tmp);
return;
}

if (php_stream_stat_path(pathto, &ssb) < 0) {
ret = php_stream_mkdir(pathto, 0777,  PHP_STREAM_MKDIR_RECURSIVE, NULL);
if (!ret) {
zend_throw_exception_ex(spl_ce_RuntimeException, 0,
"Unable to create path \"%s\" for extraction", pathto);
return;
}
} else if (!(ssb.sb.st_mode & S_IFDIR)) {
zend_throw_exception_ex(spl_ce_RuntimeException, 0,
"Unable to use path \"%s\" for extraction, it is a file, must be a directory", pathto);
return;
}

if (zval_files) {
switch (Z_TYPE_P(zval_files)) {
case IS_NULL:
goto all_files;
case IS_STRING:
filename = Z_STRVAL_P(zval_files);
filename_len = Z_STRLEN_P(zval_files);
break;
case IS_ARRAY:
nelems = zend_hash_num_elements(Z_ARRVAL_P(zval_files));
if (nelems == 0 ) {
RETURN_FALSE;
}
for (i = 0; i < nelems; i++) {
zval *zval_file;
if ((zval_file = zend_hash_index_find(Z_ARRVAL_P(zval_files), i)) != NULL) {
switch (Z_TYPE_P(zval_file)) {
case IS_STRING:
break;
default:
zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
"Invalid argument, array of filenames to extract contains non-string value");
return;
}
if (NULL == (entry = zend_hash_find_ptr(&phar_obj->archive->manifest, Z_STR_P(zval_file)))) {
zend_throw_exception_ex(phar_ce_PharException, 0,
"Phar Error: attempted to extract non-existent file \"%s\" from phar \"%s\"", Z_STRVAL_P(zval_file), phar_obj->archive->fname);
}
if (FAILURE == phar_extract_file(overwrite, entry, pathto, pathto_len, &error)) {
zend_throw_exception_ex(phar_ce_PharException, 0,
"Extraction from phar \"%s\" failed: %s", phar_obj->archive->fname, error);
efree(error);
return;
}
}
}
RETURN_TRUE;
default:
zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
"Invalid argument, expected a filename (string) or array of filenames");
return;
}

if (NULL == (entry = zend_hash_str_find_ptr(&phar_obj->archive->manifest, filename, filename_len))) {
zend_throw_exception_ex(phar_ce_PharException, 0,
"Phar Error: attempted to extract non-existent file \"%s\" from phar \"%s\"", filename, phar_obj->archive->fname);
return;
}

if (FAILURE == phar_extract_file(overwrite, entry, pathto, pathto_len, &error)) {
zend_throw_exception_ex(phar_ce_PharException, 0,
"Extraction from phar \"%s\" failed: %s", phar_obj->archive->fname, error);
efree(error);
return;
}
} else {
phar_archive_data *phar;
all_files:
phar = phar_obj->archive;
/* Extract all files */
if (!zend_hash_num_elements(&(phar->manifest))) {
RETURN_TRUE;
}

ZEND_HASH_FOREACH_PTR(&phar->manifest, entry) {
if (FAILURE == phar_extract_file(overwrite, entry, pathto, pathto_len, &error)) {
zend_throw_exception_ex(phar_ce_PharException, 0,
"Extraction from phar \"%s\" failed: %s", phar->fname, error);
efree(error);
return;
}
} ZEND_HASH_FOREACH_END();
}
RETURN_TRUE;
}
