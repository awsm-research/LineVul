static int parse_token(char **name, char **value, char **cp)
{
char *end;

if (!name || !value || !cp)
return -BLKID_ERR_PARAM;

if (!(*value = strchr(*cp, '=')))
return 0;

**value = '\0';
*name = strip_line(*cp);
*value = skip_over_blank(*value + 1);

if (**value == '"') {
		end = strchr(*value + 1, '"');
		if (!end) {
DBG(READ, ul_debug("unbalanced quotes at: %s", *value));
*cp = *value;
return -BLKID_ERR_CACHE;
}
(*value)++;
*end = '\0';
		end++;
} else {
end = skip_over_word(*value);
if (*end) {
*end = '\0';
end++;
}
}
*cp = end;

return 1;
}
