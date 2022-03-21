static int crypto_report_one(struct crypto_alg *alg,
struct crypto_user_alg *ualg, struct sk_buff *skb)
{
	strlcpy(ualg->cru_name, alg->cra_name, sizeof(ualg->cru_name));
	strlcpy(ualg->cru_driver_name, alg->cra_driver_name,
sizeof(ualg->cru_driver_name));
	strlcpy(ualg->cru_module_name, module_name(alg->cra_module),
sizeof(ualg->cru_module_name));

ualg->cru_type = 0;
ualg->cru_mask = 0;
ualg->cru_flags = alg->cra_flags;
ualg->cru_refcnt = refcount_read(&alg->cra_refcnt);

if (nla_put_u32(skb, CRYPTOCFGA_PRIORITY_VAL, alg->cra_priority))
goto nla_put_failure;
if (alg->cra_flags & CRYPTO_ALG_LARVAL) {
struct crypto_report_larval rl;

		strlcpy(rl.type, "larval", sizeof(rl.type));
if (nla_put(skb, CRYPTOCFGA_REPORT_LARVAL,
sizeof(struct crypto_report_larval), &rl))
goto nla_put_failure;
goto out;
}

if (alg->cra_type && alg->cra_type->report) {
if (alg->cra_type->report(skb, alg))
goto nla_put_failure;

goto out;
}

switch (alg->cra_flags & (CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_LARVAL)) {
case CRYPTO_ALG_TYPE_CIPHER:
if (crypto_report_cipher(skb, alg))
goto nla_put_failure;

break;
case CRYPTO_ALG_TYPE_COMPRESS:
if (crypto_report_comp(skb, alg))
goto nla_put_failure;

break;
case CRYPTO_ALG_TYPE_ACOMPRESS:
if (crypto_report_acomp(skb, alg))
goto nla_put_failure;

break;
case CRYPTO_ALG_TYPE_AKCIPHER:
if (crypto_report_akcipher(skb, alg))
goto nla_put_failure;

break;
case CRYPTO_ALG_TYPE_KPP:
if (crypto_report_kpp(skb, alg))
goto nla_put_failure;
break;
}

out:
return 0;

nla_put_failure:
return -EMSGSIZE;
}
