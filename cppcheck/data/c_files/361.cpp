cJSON *cJSON_CreateObject( void )
{
	cJSON *item = cJSON_New_Item();
	if ( item )
		item->type = cJSON_Object;
	return item;
}
