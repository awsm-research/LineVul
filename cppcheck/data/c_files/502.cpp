static void _php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAMETERS, OnigOptionType options, int is_callable)
{
zval **arg_pattern_zval;

char *arg_pattern;
int arg_pattern_len;

char *replace;
int replace_len;

zend_fcall_info arg_replace_fci;
zend_fcall_info_cache arg_replace_fci_cache;

char *string;
int string_len;

char *p;
php_mb_regex_t *re;
OnigSyntaxType *syntax;
OnigRegion *regs = NULL;
smart_str out_buf = { 0 };
smart_str eval_buf = { 0 };
smart_str *pbuf;
int i, err, eval, n;
OnigUChar *pos;
OnigUChar *string_lim;
char *description = NULL;
char pat_buf[2];

const mbfl_encoding *enc;

{
const char *current_enc_name;
current_enc_name = _php_mb_regex_mbctype2name(MBREX(current_mbctype));
if (current_enc_name == NULL ||
(enc = mbfl_name2encoding(current_enc_name)) == NULL) {
php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown error");
RETURN_FALSE;
}
}
eval = 0;
{
char *option_str = NULL;
int option_str_len = 0;

if (!is_callable) {
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zss|s",
&arg_pattern_zval,
&replace, &replace_len,
&string, &string_len,
&option_str, &option_str_len) == FAILURE) {
RETURN_FALSE;
}
} else {
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zfs|s",
&arg_pattern_zval,
&arg_replace_fci, &arg_replace_fci_cache,
&string, &string_len,
&option_str, &option_str_len) == FAILURE) {
RETURN_FALSE;
}
}

if (option_str != NULL) {
_php_mb_regex_init_options(option_str, option_str_len, &options, &syntax, &eval);
} else {
options |= MBREX(regex_default_options);
syntax = MBREX(regex_default_syntax);
}
}
if (Z_TYPE_PP(arg_pattern_zval) == IS_STRING) {
arg_pattern = Z_STRVAL_PP(arg_pattern_zval);
arg_pattern_len = Z_STRLEN_PP(arg_pattern_zval);
} else {
/* FIXME: this code is not multibyte aware! */
convert_to_long_ex(arg_pattern_zval);
		pat_buf[0] = (char)Z_LVAL_PP(arg_pattern_zval);	
pat_buf[1] = '\0';

arg_pattern = pat_buf;
		arg_pattern_len = 1;	
}
/* create regex pattern buffer */
re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, options, MBREX(current_mbctype), syntax TSRMLS_CC);
if (re == NULL) {
RETURN_FALSE;
}

if (eval || is_callable) {
pbuf = &eval_buf;
description = zend_make_compiled_string_description("mbregex replace" TSRMLS_CC);
} else {
pbuf = &out_buf;
description = NULL;
}

if (is_callable) {
if (eval) {
php_error_docref(NULL TSRMLS_CC, E_WARNING, "Option 'e' cannot be used with replacement callback");
RETURN_FALSE;
}
}

/* do the actual work */
err = 0;
pos = (OnigUChar *)string;
string_lim = (OnigUChar*)(string + string_len);
regs = onig_region_new();
while (err >= 0) {
err = onig_search(re, (OnigUChar *)string, (OnigUChar *)string_lim, pos, (OnigUChar *)string_lim, regs, 0);
if (err <= -2) {
OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
onig_error_code_to_str(err_str, err);
php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in php_mbereg_replace_exec(): %s", err_str);
break;
}
if (err >= 0) {
#if moriyoshi_0
if (regs->beg[0] == regs->end[0]) {
php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
break;
}
#endif
/* copy the part of the string before the match */
smart_str_appendl(&out_buf, pos, (size_t)((OnigUChar *)(string + regs->beg[0]) - pos));

if (!is_callable) {
/* copy replacement and backrefs */
i = 0;
p = replace;
while (i < replace_len) {
int fwd = (int) php_mb_mbchar_bytes_ex(p, enc);
n = -1;
if ((replace_len - i) >= 2 && fwd == 1 &&
p[0] == '\\' && p[1] >= '0' && p[1] <= '9') {
n = p[1] - '0';
}
if (n >= 0 && n < regs->num_regs) {
if (regs->beg[n] >= 0 && regs->beg[n] < regs->end[n] && regs->end[n] <= string_len) {
smart_str_appendl(pbuf, string + regs->beg[n], regs->end[n] - regs->beg[n]);
}
p += 2;
i += 2;
} else {
smart_str_appendl(pbuf, p, fwd);
p += fwd;
i += fwd;
}
}
}
				
if (eval) {
zval v;
/* null terminate buffer */
smart_str_0(&eval_buf);
/* do eval */
if (zend_eval_stringl(eval_buf.c, eval_buf.len, &v, description TSRMLS_CC) == FAILURE) {
efree(description);
php_error_docref(NULL TSRMLS_CC,E_ERROR, "Failed evaluating code: %s%s", PHP_EOL, eval_buf.c);
/* zend_error() does not return in this case */
}

/* result of eval */
convert_to_string(&v);
smart_str_appendl(&out_buf, Z_STRVAL(v), Z_STRLEN(v));
/* Clean up */
eval_buf.len = 0;
zval_dtor(&v);
} else if (is_callable) {
				zval *retval_ptr;
zval **args[1];
zval *subpats;
int i;
				
MAKE_STD_ZVAL(subpats);
array_init(subpats);
				
for (i = 0; i < regs->num_regs; i++) {
add_next_index_stringl(subpats, string + regs->beg[i], regs->end[i] - regs->beg[i], 1);
				}				
				
args[0] = &subpats;
/* null terminate buffer */
smart_str_0(&eval_buf);
				
arg_replace_fci.param_count = 1;
arg_replace_fci.params = args;
arg_replace_fci.retval_ptr_ptr = &retval_ptr;
				if (zend_call_function(&arg_replace_fci, &arg_replace_fci_cache TSRMLS_CC) == SUCCESS && arg_replace_fci.retval_ptr_ptr) {
convert_to_string_ex(&retval_ptr);
smart_str_appendl(&out_buf, Z_STRVAL_P(retval_ptr), Z_STRLEN_P(retval_ptr));
eval_buf.len = 0;
zval_ptr_dtor(&retval_ptr);
} else {
					efree(description);
if (!EG(exception)) {
php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call custom replacement function");
}
}
zval_ptr_dtor(&subpats);
}

n = regs->end[0];
if ((pos - (OnigUChar *)string) < n) {
pos = (OnigUChar *)string + n;
} else {
if (pos < string_lim) {
					smart_str_appendl(&out_buf, pos, 1); 
}
pos++;
}
} else { /* nomatch */
/* stick that last bit of string on our output */
if (string_lim - pos > 0) {
smart_str_appendl(&out_buf, pos, string_lim - pos);
}
}
onig_region_free(regs, 0);
}

if (description) {
efree(description);
}
if (regs != NULL) {
onig_region_free(regs, 1);
}
smart_str_free(&eval_buf);

if (err <= -2) {
		smart_str_free(&out_buf);	
RETVAL_FALSE;
} else {
smart_str_appendc(&out_buf, '\0');
RETVAL_STRINGL((char *)out_buf.c, out_buf.len - 1, 0);
}
}
