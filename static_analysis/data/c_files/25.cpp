my_object_dict_of_dicts (MyObject *obj, GHashTable *in,
                                GHashTable **out, GError **error)
{
  *out = g_hash_table_new_full (g_str_hash, g_str_equal,
				(GDestroyNotify) g_free,
                                (GDestroyNotify) g_hash_table_destroy);
  g_hash_table_foreach (in, hash_foreach_mangle_dict_of_strings, *out);
  return TRUE;
}
