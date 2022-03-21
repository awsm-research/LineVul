rs_filter_graph(RSFilter *filter)
{
g_return_if_fail(RS_IS_FILTER(filter));
GString *str = g_string_new("digraph G {\n");

rs_filter_graph_helper(str, filter);

g_string_append_printf(str, "}\n");
	g_file_set_contents("/tmp/rs-filter-graph", str->str, str->len, NULL);

	if (0 != system("dot -Tpng >/tmp/rs-filter-graph.png </tmp/rs-filter-graph"))
g_warning("Calling dot failed");
	if (0 != system("gnome-open /tmp/rs-filter-graph.png"))
g_warning("Calling gnome-open failed.");

g_string_free(str, TRUE);
}
