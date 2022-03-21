cJSON *cJSON_DetachItemFromArray( cJSON *array, int which )
{
	cJSON *c = array->child;
	while ( c && which > 0 ) {
		c = c->next;
		--which;
	}
	if ( ! c )
		return 0;
	if ( c->prev )
		c->prev->next = c->next;
	if ( c->next ) c->next->prev = c->prev;
	if ( c == array->child )
		array->child = c->next;
	c->prev = c->next = 0;
	return c;
}
