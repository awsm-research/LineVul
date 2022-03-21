static void php_wddx_pop_element(void *user_data, const XML_Char *name)
{
st_entry 			*ent1, *ent2;
wddx_stack 			*stack = (wddx_stack *)user_data;
HashTable 			*target_hash;
zend_class_entry 	**pce;
zval				*obj;
zval				*tmp;
TSRMLS_FETCH();

/* OBJECTS_FIXME */
if (stack->top == 0) {
return;
}

if (!strcmp(name, EL_STRING) || !strcmp(name, EL_NUMBER) ||
!strcmp(name, EL_BOOLEAN) || !strcmp(name, EL_NULL) ||
!strcmp(name, EL_ARRAY) || !strcmp(name, EL_STRUCT) ||
!strcmp(name, EL_RECORDSET) || !strcmp(name, EL_BINARY) ||
!strcmp(name, EL_DATETIME)) {
wddx_stack_top(stack, (void**)&ent1);

if (!ent1->data) {
if (stack->top > 1) {
stack->top--;
} else {
stack->done = 1;
}
			efree(ent1);
return;
}

if (!strcmp(name, EL_BINARY)) {
int new_len=0;
unsigned char *new_str;

new_str = php_base64_decode(Z_STRVAL_P(ent1->data), Z_STRLEN_P(ent1->data), &new_len);
STR_FREE(Z_STRVAL_P(ent1->data));
if (new_str) {
Z_STRVAL_P(ent1->data) = new_str;
Z_STRLEN_P(ent1->data) = new_len;
} else {
ZVAL_EMPTY_STRING(ent1->data);
}
}

/* Call __wakeup() method on the object. */
if (Z_TYPE_P(ent1->data) == IS_OBJECT) {
zval *fname, *retval = NULL;

MAKE_STD_ZVAL(fname);
ZVAL_STRING(fname, "__wakeup", 1);

call_user_function_ex(NULL, &ent1->data, fname, &retval, 0, 0, 0, NULL TSRMLS_CC);

zval_dtor(fname);
FREE_ZVAL(fname);
if (retval) {
zval_ptr_dtor(&retval);
}
}

if (stack->top > 1) {
stack->top--;
wddx_stack_top(stack, (void**)&ent2);

/* if non-existent field */
			if (ent2->type == ST_FIELD && ent2->data == NULL) {
zval_ptr_dtor(&ent1->data);
efree(ent1);
return;
}

if (Z_TYPE_P(ent2->data) == IS_ARRAY || Z_TYPE_P(ent2->data) == IS_OBJECT) {
target_hash = HASH_OF(ent2->data);

if (ent1->varname) {
if (!strcmp(ent1->varname, PHP_CLASS_NAME_VAR) &&
Z_TYPE_P(ent1->data) == IS_STRING && Z_STRLEN_P(ent1->data) &&
ent2->type == ST_STRUCT && Z_TYPE_P(ent2->data) == IS_ARRAY) {
zend_bool incomplete_class = 0;

zend_str_tolower(Z_STRVAL_P(ent1->data), Z_STRLEN_P(ent1->data));
if (zend_hash_find(EG(class_table), Z_STRVAL_P(ent1->data),
Z_STRLEN_P(ent1->data)+1, (void **) &pce)==FAILURE) {
incomplete_class = 1;
pce = &PHP_IC_ENTRY;
}

/* Initialize target object */
MAKE_STD_ZVAL(obj);
object_init_ex(obj, *pce);

/* Merge current hashtable with object's default properties */
zend_hash_merge(Z_OBJPROP_P(obj),
Z_ARRVAL_P(ent2->data),
(void (*)(void *)) zval_add_ref,
(void *) &tmp, sizeof(zval *), 0);

if (incomplete_class) {
php_store_class_name(obj, Z_STRVAL_P(ent1->data), Z_STRLEN_P(ent1->data));
}

/* Clean up old array entry */
zval_ptr_dtor(&ent2->data);

/* Set stack entry to point to the newly created object */
ent2->data = obj;

/* Clean up class name var entry */
zval_ptr_dtor(&ent1->data);
} else if (Z_TYPE_P(ent2->data) == IS_OBJECT) {
zend_class_entry *old_scope = EG(scope);

EG(scope) = Z_OBJCE_P(ent2->data);
Z_DELREF_P(ent1->data);
add_property_zval(ent2->data, ent1->varname, ent1->data);
EG(scope) = old_scope;
} else {
zend_symtable_update(target_hash, ent1->varname, strlen(ent1->varname)+1, &ent1->data, sizeof(zval *), NULL);
}
efree(ent1->varname);
} else	{
zend_hash_next_index_insert(target_hash, &ent1->data, sizeof(zval *), NULL);
}
}
efree(ent1);
} else {
stack->done = 1;
}
} else if (!strcmp(name, EL_VAR) && stack->varname) {
efree(stack->varname);
stack->varname = NULL;
} else if (!strcmp(name, EL_FIELD)) {
st_entry *ent;
wddx_stack_top(stack, (void **)&ent);
efree(ent);
stack->top--;
}
}
