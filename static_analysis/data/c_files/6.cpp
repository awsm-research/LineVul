static int compile_search(struct Context *ctx, const struct Pattern *pat, struct Buffer *buf)
{
if (do_search(pat, 0) == 0)
return 0;

if (pat->not)
mutt_buffer_addstr(buf, "NOT ");

if (pat->child)
{
int clauses;

clauses = do_search(pat->child, 1);
if (clauses > 0)
{
const struct Pattern *clause = pat->child;

mutt_buffer_addch(buf, '(');

while (clauses)
{
if (do_search(clause, 0))
{
if (pat->op == MUTT_OR && clauses > 1)
mutt_buffer_addstr(buf, "OR ");
clauses--;

if (compile_search(ctx, clause, buf) < 0)
return -1;

if (clauses)
mutt_buffer_addch(buf, ' ');
}
clause = clause->next;
}

mutt_buffer_addch(buf, ')');
}
}
else
{
char term[STRING];
char *delim = NULL;

switch (pat->op)
{
case MUTT_HEADER:
mutt_buffer_addstr(buf, "HEADER ");

/* extract header name */
delim = strchr(pat->p.str, ':');
if (!delim)
{
mutt_error(_("Header search without header name: %s"), pat->p.str);
return -1;
}
*delim = '\0';
        imap_quote_string(term, sizeof(term), pat->p.str);
mutt_buffer_addstr(buf, term);
mutt_buffer_addch(buf, ' ');

/* and field */
*delim = ':';
delim++;
SKIPWS(delim);
        imap_quote_string(term, sizeof(term), delim);
mutt_buffer_addstr(buf, term);
break;
case MUTT_BODY:
mutt_buffer_addstr(buf, "BODY ");
        imap_quote_string(term, sizeof(term), pat->p.str);
mutt_buffer_addstr(buf, term);
break;
case MUTT_WHOLE_MSG:
mutt_buffer_addstr(buf, "TEXT ");
        imap_quote_string(term, sizeof(term), pat->p.str);
mutt_buffer_addstr(buf, term);
break;
case MUTT_SERVERSEARCH:
{
struct ImapData *idata = ctx->data;
if (!mutt_bit_isset(idata->capabilities, X_GM_EXT1))
{
mutt_error(_("Server-side custom search not supported: %s"), pat->p.str);
return -1;
}
}
mutt_buffer_addstr(buf, "X-GM-RAW ");
        imap_quote_string(term, sizeof(term), pat->p.str);
mutt_buffer_addstr(buf, term);
break;
}
}

return 0;
}
