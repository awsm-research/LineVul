PHP_FUNCTION(imagesetstyle)
{
zval *IM, *styles;
gdImagePtr im;
int * stylearr;
int index;
HashPosition pos;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &IM, &styles) == FAILURE)  {
return;
}

ZEND_FETCH_RESOURCE(im, gdImagePtr, &IM, -1, "Image", le_gd);

/* copy the style values in the stylearr */
stylearr = safe_emalloc(sizeof(int), zend_hash_num_elements(HASH_OF(styles)), 0);

zend_hash_internal_pointer_reset_ex(HASH_OF(styles), &pos);

for (index = 0;; zend_hash_move_forward_ex(HASH_OF(styles), &pos))	{
zval ** item;

if (zend_hash_get_current_data_ex(HASH_OF(styles), (void **) &item, &pos) == FAILURE) {
break;
}

		convert_to_long_ex(item);
		stylearr[index++] = Z_LVAL_PP(item);
}

gdImageSetStyle(im, stylearr, index);

efree(stylearr);

RETURN_TRUE;
}
