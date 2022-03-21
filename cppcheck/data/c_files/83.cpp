static int init_nss_hash(struct crypto_instance *instance)
{
PK11SlotInfo*	hash_slot = NULL;
SECItem		hash_param;

if (!hash_to_nss[instance->crypto_hash_type]) {
return 0;
}

hash_param.type = siBuffer;
	hash_param.data = 0;
	hash_param.len = 0;

hash_slot = PK11_GetBestSlot(hash_to_nss[instance->crypto_hash_type], NULL);
if (hash_slot == NULL) {
log_printf(instance->log_level_security, "Unable to find security slot (err %d)",
PR_GetError());
return -1;
}

instance->nss_sym_key_sign = PK11_ImportSymKey(hash_slot,
hash_to_nss[instance->crypto_hash_type],
PK11_OriginUnwrap, CKA_SIGN,
&hash_param, NULL);
if (instance->nss_sym_key_sign == NULL) {
log_printf(instance->log_level_security, "Failure to import key into NSS (err %d)",
PR_GetError());
return -1;
}

PK11_FreeSlot(hash_slot);

return 0;
}
