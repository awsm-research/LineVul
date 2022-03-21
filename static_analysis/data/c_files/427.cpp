my_object_str_hash_len (MyObject *obj, GHashTable *table, guint *len, GError **error)
{
  *len = 0;
  g_hash_table_foreach (table, hash_foreach, len);
  return TRUE;
}
