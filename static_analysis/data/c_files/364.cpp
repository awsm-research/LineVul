static void spl_filesystem_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
spl_filesystem_object *intern = (spl_filesystem_object*)object;

if (intern->oth_handler && intern->oth_handler->dtor) {
intern->oth_handler->dtor(intern TSRMLS_CC);
}
	
zend_object_std_dtor(&intern->std TSRMLS_CC);
	
if (intern->_path) {
efree(intern->_path);
}
if (intern->file_name) {
efree(intern->file_name);
}
switch(intern->type) {
case SPL_FS_INFO:
break;
case SPL_FS_DIR:
if (intern->u.dir.dirp) {
php_stream_close(intern->u.dir.dirp);
intern->u.dir.dirp = NULL;
}
if (intern->u.dir.sub_path) {
efree(intern->u.dir.sub_path);
		}		
break;
case SPL_FS_FILE:
if (intern->u.file.stream) {
if (intern->u.file.zcontext) {
/*				zend_list_delref(Z_RESVAL_P(intern->zcontext));*/
}
if (!intern->u.file.stream->is_persistent) {
php_stream_free(intern->u.file.stream, PHP_STREAM_FREE_CLOSE);
} else {
php_stream_free(intern->u.file.stream, PHP_STREAM_FREE_CLOSE_PERSISTENT);
}
if (intern->u.file.open_mode) {
efree(intern->u.file.open_mode);
}
if (intern->orig_path) {
efree(intern->orig_path);
}
}
spl_filesystem_file_free_line(intern TSRMLS_CC);
break;
}

{
zend_object_iterator *iterator;
iterator = (zend_object_iterator*)
spl_filesystem_object_to_iterator(intern);
if (iterator->data != NULL) {
iterator->data = NULL;
iterator->funcs->dtor(iterator TSRMLS_CC);
}
}
efree(object);
} /* }}} */
