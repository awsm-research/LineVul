void cJSON_AddItemReferenceToObject( cJSON *object, const char *string, cJSON *item )
{
	cJSON_AddItemToObject( object, string, create_reference( item ) );
}
