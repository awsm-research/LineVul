static void Sp_search(js_State *J)
{
js_Regexp *re;
const char *text;
Resub m;

text = checkstring(J, 0);

if (js_isregexp(J, 1))
js_copy(J, 1);
else if (js_isundefined(J, 1))
js_newregexp(J, "", 0);
else
js_newregexp(J, js_tostring(J, 1), 0);

re = js_toregexp(J, -1);

	if (!js_regexec(re->prog, text, &m, 0))
js_pushnumber(J, js_utfptrtoidx(text, m.sub[0].sp));
else
js_pushnumber(J, -1);
}
