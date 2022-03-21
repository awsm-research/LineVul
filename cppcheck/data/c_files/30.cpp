SPL_METHOD(SplFileObject, seek)
{
spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
long line_pos;
	
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &line_pos) == FAILURE) {
return;
}
if (line_pos < 0) {
zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Can't seek file %s to negative line %ld", intern->file_name, line_pos);
		RETURN_FALSE;		
}
	
spl_filesystem_file_rewind(getThis(), intern TSRMLS_CC);
	
while(intern->u.file.current_line_num < line_pos) {
if (spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC) == FAILURE) {
break;
}
}
} /* }}} */

/* {{{ Function/Class/Method definitions */
