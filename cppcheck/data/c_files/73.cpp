cleanup_pathname(struct archive_write_disk *a)
{
char *dest, *src;
char separator = '\0';

dest = src = a->name;
if (*src == '\0') {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Invalid empty pathname");
return (ARCHIVE_FAILED);
}

#if defined(__CYGWIN__)
cleanup_pathname_win(a);
#endif
/* Skip leading '/'. */
	if (*src == '/')
separator = *src++;

/* Scan the pathname one element at a time. */
for (;;) {
/* src points to first char after '/' */
if (src[0] == '\0') {
break;
} else if (src[0] == '/') {
/* Found '//', ignore second one. */
src++;
continue;
} else if (src[0] == '.') {
if (src[1] == '\0') {
/* Ignore trailing '.' */
break;
} else if (src[1] == '/') {
/* Skip './'. */
src += 2;
continue;
} else if (src[1] == '.') {
if (src[2] == '/' || src[2] == '\0') {
/* Conditionally warn about '..' */
if (a->flags & ARCHIVE_EXTRACT_SECURE_NODOTDOT) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC,
"Path contains '..'");
return (ARCHIVE_FAILED);
}
}
/*
* Note: Under no circumstances do we
* remove '..' elements.  In
* particular, restoring
* '/foo/../bar/' should create the
* 'foo' dir as a side-effect.
*/
}
}

/* Copy current element, including leading '/'. */
if (separator)
*dest++ = '/';
while (*src != '\0' && *src != '/') {
*dest++ = *src++;
}

if (*src == '\0')
break;

/* Skip '/' separator. */
separator = *src++;
}
/*
* We've just copied zero or more path elements, not including the
* final '/'.
*/
if (dest == a->name) {
/*
* Nothing got copied.  The path must have been something
* like '.' or '/' or './' or '/././././/./'.
*/
if (separator)
*dest++ = '/';
else
*dest++ = '.';
}
/* Terminate the result. */
*dest = '\0';
return (ARCHIVE_OK);
}
