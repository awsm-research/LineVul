static void php_wddx_process_data(void *user_data, const XML_Char *s, int len)
{
st_entry *ent;
wddx_stack *stack = (wddx_stack *)user_data;
TSRMLS_FETCH();

if (!wddx_stack_is_empty(stack) && !stack->done) {
wddx_stack_top(stack, (void**)&ent);
switch (ent->type) {
case ST_STRING:
if (Z_STRLEN_P(ent->data) == 0) {
STR_FREE(Z_STRVAL_P(ent->data));
Z_STRVAL_P(ent->data) = estrndup(s, len);
Z_STRLEN_P(ent->data) = len;
} else {
Z_STRVAL_P(ent->data) = erealloc(Z_STRVAL_P(ent->data), Z_STRLEN_P(ent->data) + len + 1);
memcpy(Z_STRVAL_P(ent->data) + Z_STRLEN_P(ent->data), s, len);
Z_STRLEN_P(ent->data) += len;
Z_STRVAL_P(ent->data)[Z_STRLEN_P(ent->data)] = '\0';
}
break;

case ST_BINARY:
if (Z_STRLEN_P(ent->data) == 0) {
STR_FREE(Z_STRVAL_P(ent->data));
Z_STRVAL_P(ent->data) = estrndup(s, len + 1);
} else {
Z_STRVAL_P(ent->data) = erealloc(Z_STRVAL_P(ent->data), Z_STRLEN_P(ent->data) + len + 1);
memcpy(Z_STRVAL_P(ent->data) + Z_STRLEN_P(ent->data), s, len);
}
Z_STRLEN_P(ent->data) += len;
Z_STRVAL_P(ent->data)[Z_STRLEN_P(ent->data)] = '\0';
break;

case ST_NUMBER:
Z_TYPE_P(ent->data) = IS_STRING;
Z_STRLEN_P(ent->data) = len;
Z_STRVAL_P(ent->data) = estrndup(s, len);
convert_scalar_to_number(ent->data TSRMLS_CC);
break;

case ST_BOOLEAN:
if(!ent->data) {
break;
}
if (!strcmp(s, "true")) {
Z_LVAL_P(ent->data) = 1;
} else if (!strcmp(s, "false")) {
Z_LVAL_P(ent->data) = 0;
} else {
zval_ptr_dtor(&ent->data);
if (ent->varname) {
efree(ent->varname);
ent->varname = NULL;
}
ent->data = NULL;
}
break;

case ST_DATETIME: {
char *tmp;

				tmp = emalloc(len + 1);
				memcpy(tmp, s, len);
tmp[len] = '\0';

Z_LVAL_P(ent->data) = php_parse_date(tmp, NULL);
/* date out of range < 1969 or > 2038 */
if (Z_LVAL_P(ent->data) == -1) {
					Z_TYPE_P(ent->data) = IS_STRING;
					Z_STRLEN_P(ent->data) = len;
					Z_STRVAL_P(ent->data) = estrndup(s, len);
}
				efree(tmp);
}
break;

default:
break;
}
}
}
