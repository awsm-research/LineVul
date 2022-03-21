safecat_color_encoding(char *buffer, size_t bufsize, size_t pos,
   PNG_CONST color_encoding *e, double encoding_gamma)
{
if (e != 0)
{
if (encoding_gamma != 0)
pos = safecat(buffer, bufsize, pos, "(");
pos = safecat(buffer, bufsize, pos, "R(");
pos = safecatd(buffer, bufsize, pos, e->red.X, 4);
pos = safecat(buffer, bufsize, pos, ",");
pos = safecatd(buffer, bufsize, pos, e->red.Y, 4);
pos = safecat(buffer, bufsize, pos, ",");
pos = safecatd(buffer, bufsize, pos, e->red.Z, 4);
pos = safecat(buffer, bufsize, pos, "),G(");
pos = safecatd(buffer, bufsize, pos, e->green.X, 4);
pos = safecat(buffer, bufsize, pos, ",");
pos = safecatd(buffer, bufsize, pos, e->green.Y, 4);
pos = safecat(buffer, bufsize, pos, ",");
pos = safecatd(buffer, bufsize, pos, e->green.Z, 4);
pos = safecat(buffer, bufsize, pos, "),B(");
pos = safecatd(buffer, bufsize, pos, e->blue.X, 4);
pos = safecat(buffer, bufsize, pos, ",");
pos = safecatd(buffer, bufsize, pos, e->blue.Y, 4);
pos = safecat(buffer, bufsize, pos, ",");
pos = safecatd(buffer, bufsize, pos, e->blue.Z, 4);
pos = safecat(buffer, bufsize, pos, ")");
if (encoding_gamma != 0)
pos = safecat(buffer, bufsize, pos, ")");
}

if (encoding_gamma != 0)
{
pos = safecat(buffer, bufsize, pos, "^");
pos = safecatd(buffer, bufsize, pos, encoding_gamma, 5);
}


return pos;
}
