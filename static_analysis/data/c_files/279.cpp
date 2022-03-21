my_object_many_args (MyObject *obj, guint32 x, const char *str, double trouble, double *d_ret, char **str_ret, GError **error)
{
  *d_ret = trouble + (x * 2);
  *str_ret = g_ascii_strup (str, -1);
  return TRUE;
}
