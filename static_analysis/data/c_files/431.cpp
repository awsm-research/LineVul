static zval *xml_call_handler(xml_parser *parser, zval *handler, zend_function *function_ptr, int argc, zval **argv)
{
       int i;  
TSRMLS_FETCH();

if (parser && handler && !EG(exception)) {
zval ***args;
zval *retval;
int result;
zend_fcall_info fci;

args = safe_emalloc(sizeof(zval **), argc, 0);
for (i = 0; i < argc; i++) {
args[i] = &argv[i];
}
               
fci.size = sizeof(fci);
fci.function_table = EG(function_table);
fci.function_name = handler;
fci.symbol_table = NULL;
fci.object_ptr = parser->object;
fci.retval_ptr_ptr = &retval;
fci.param_count = argc;
fci.params = args;
fci.no_separation = 0;
/*fci.function_handler_cache = &function_ptr;*/

result = zend_call_function(&fci, NULL TSRMLS_CC);
if (result == FAILURE) {
zval **method;
zval **obj;

if (Z_TYPE_P(handler) == IS_STRING) {
php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler %s()", Z_STRVAL_P(handler));
} else if (zend_hash_index_find(Z_ARRVAL_P(handler), 0, (void **) &obj) == SUCCESS &&
zend_hash_index_find(Z_ARRVAL_P(handler), 1, (void **) &method) == SUCCESS &&
Z_TYPE_PP(obj) == IS_OBJECT &&
Z_TYPE_PP(method) == IS_STRING) {
php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler %s::%s()", Z_OBJCE_PP(obj)->name, Z_STRVAL_PP(method));
                       } else 
php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler");
}

for (i = 0; i < argc; i++) {
zval_ptr_dtor(args[i]);
}
efree(args);

if (result == FAILURE) {
return NULL;
} else {
return EG(exception) ? NULL : retval;
}
} else {
for (i = 0; i < argc; i++) {
zval_ptr_dtor(&argv[i]);
}
return NULL;
}
}
