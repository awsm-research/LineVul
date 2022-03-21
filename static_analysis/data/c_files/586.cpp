static int svc_can_register(const uint16_t *name, size_t name_len, pid_t spid, uid_t uid)

{
const char *perm = "add";

    if (uid >= AID_APP) {
return 0; /* Don't allow apps to register services */
}

return check_mac_perms_from_lookup(spid, uid, perm, str8(name, name_len)) ? 1 : 0;
}
