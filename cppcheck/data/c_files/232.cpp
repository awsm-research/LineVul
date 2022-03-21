static int generate_key(DH *dh)
{
int ok = 0;
int generate_new_key = 0;
unsigned l;
    BN_CTX *ctx;
BN_MONT_CTX *mont = NULL;
BIGNUM *pub_key = NULL, *priv_key = NULL;

ctx = BN_CTX_new();
if (ctx == NULL)
goto err;
generate_new_key = 1;
} else
