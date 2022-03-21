int BN_hex2bn(BIGNUM **bn, const char *a)
{
BIGNUM *ret = NULL;
BN_ULONG l = 0;
int neg = 0, h, m, i, j, k, c;
int num;

if ((a == NULL) || (*a == '\0'))
return (0);

if (*a == '-') {
neg = 1;
a++;
a++;
}

    for (i = 0; isxdigit((unsigned char)a[i]); i++) ;

num = i + neg;
if (bn == NULL)
return (0);
} else {
ret = *bn;
BN_zero(ret);
}
