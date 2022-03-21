ptaReadStream(FILE  *fp)
{
char       typestr[128];
l_int32    i, n, ix, iy, type, version;
l_float32  x, y;
PTA       *pta;

PROCNAME("ptaReadStream");

if (!fp)
return (PTA *)ERROR_PTR("stream not defined", procName, NULL);

if (fscanf(fp, "\n Pta Version %d\n", &version) != 1)
return (PTA *)ERROR_PTR("not a pta file", procName, NULL);
if (version != PTA_VERSION_NUMBER)
return (PTA *)ERROR_PTR("invalid pta version", procName, NULL);
    if (fscanf(fp, " Number of pts = %d; format = %s\n", &n, typestr) != 2)
return (PTA *)ERROR_PTR("not a pta file", procName, NULL);
if (!strcmp(typestr, "float"))
type = 0;
else  /* typestr is "integer" */
type = 1;

if ((pta = ptaCreate(n)) == NULL)
return (PTA *)ERROR_PTR("pta not made", procName, NULL);
for (i = 0; i < n; i++) {
if (type == 0) {  /* data is float */
if (fscanf(fp, "   (%f, %f)\n", &x, &y) != 2) {
ptaDestroy(&pta);
return (PTA *)ERROR_PTR("error reading floats", procName, NULL);
}
ptaAddPt(pta, x, y);
} else {   /* data is integer */
if (fscanf(fp, "   (%d, %d)\n", &ix, &iy) != 2) {
ptaDestroy(&pta);
return (PTA *)ERROR_PTR("error reading ints", procName, NULL);
}
ptaAddPt(pta, ix, iy);
}
}

return pta;
}
