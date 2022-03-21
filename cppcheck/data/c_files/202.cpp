check_1_6_dummy(kadm5_principal_ent_t entry, long mask,
int n_ks_tuple, krb5_key_salt_tuple *ks_tuple, char **passptr)
{
int i;
char *password = *passptr;

/* Old-style randkey operations disallowed tickets to start. */
    if (!(mask & KADM5_ATTRIBUTES) ||
!(entry->attributes & KRB5_KDB_DISALLOW_ALL_TIX))
return;

/* The 1.6 dummy password was the octets 1..255. */
for (i = 0; (unsigned char) password[i] == i + 1; i++);
if (password[i] != '\0' || i != 255)
return;

/* This will make the caller use a random password instead. */
*passptr = NULL;
}
