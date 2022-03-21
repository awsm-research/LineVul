static cJSON *get_object_item(const cJSON * const object, const char * const name, const cJSON_bool case_sensitive)
{
cJSON *current_element = NULL;

if ((object == NULL) || (name == NULL))
{
return NULL;
}

current_element = object->child;
if (case_sensitive)
{
        while ((current_element != NULL) && (strcmp(name, current_element->string) != 0))
{
current_element = current_element->next;
}
}
else
{
while ((current_element != NULL) && (case_insensitive_strcmp((const unsigned char*)name, (const unsigned char*)(current_element->string)) != 0))
{
current_element = current_element->next;
}
}

return current_element;
}
