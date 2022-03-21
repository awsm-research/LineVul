_exsltDateTruncateDate (exsltDateValPtr dt, exsltDateType type)
{
if (dt == NULL)
return 1;

if ((type & XS_TIME) != XS_TIME) {
dt->value.date.hour = 0;
dt->value.date.min  = 0;
dt->value.date.sec  = 0.0;
}

if ((type & XS_GDAY) != XS_GDAY)
        dt->value.date.day = 0;

if ((type & XS_GMONTH) != XS_GMONTH)
        dt->value.date.mon = 0;

if ((type & XS_GYEAR) != XS_GYEAR)
dt->value.date.year = 0;

dt->type = type;

return 0;
}
