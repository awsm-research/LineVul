gplotAddPlot(GPLOT       *gplot,
NUMA        *nax,
NUMA        *nay,
l_int32      plotstyle,
const char  *plottitle)
{
char       buf[L_BUF_SIZE];
char       emptystring[] = "";
char      *datastr, *title;
l_int32    n, i;
l_float32  valx, valy, startx, delx;
SARRAY    *sa;

PROCNAME("gplotAddPlot");

if (!gplot)
return ERROR_INT("gplot not defined", procName, 1);
if (!nay)
return ERROR_INT("nay not defined", procName, 1);
if (plotstyle < 0 || plotstyle >= NUM_GPLOT_STYLES)
return ERROR_INT("invalid plotstyle", procName, 1);

if ((n = numaGetCount(nay)) == 0)
return ERROR_INT("no points to plot", procName, 1);
if (nax && (n != numaGetCount(nax)))
return ERROR_INT("nax and nay sizes differ", procName, 1);
if (n == 1 && plotstyle == GPLOT_LINES) {
L_INFO("only 1 pt; changing style to points\n", procName);
plotstyle = GPLOT_POINTS;
}

/* Save plotstyle and plottitle */
numaGetParameters(nay, &startx, &delx);
numaAddNumber(gplot->plotstyles, plotstyle);
if (plottitle) {
title = stringNew(plottitle);
sarrayAddString(gplot->plottitles, title, L_INSERT);
} else {
sarrayAddString(gplot->plottitles, emptystring, L_COPY);
}

/* Generate and save data filename */
gplot->nplots++;
    snprintf(buf, L_BUF_SIZE, "%s.data.%d", gplot->rootname, gplot->nplots);
sarrayAddString(gplot->datanames, buf, L_COPY);

/* Generate data and save as a string */
sa = sarrayCreate(n);
for (i = 0; i < n; i++) {
if (nax)
numaGetFValue(nax, i, &valx);
else
valx = startx + i * delx;
numaGetFValue(nay, i, &valy);
        snprintf(buf, L_BUF_SIZE, "%f %f\n", valx, valy);
sarrayAddString(sa, buf, L_COPY);
}
datastr = sarrayToString(sa, 0);
sarrayAddString(gplot->plotdata, datastr, L_INSERT);
sarrayDestroy(&sa);

return 0;
}
