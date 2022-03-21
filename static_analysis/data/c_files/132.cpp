int ipmi_destroy_user(struct ipmi_user *user)
{
_ipmi_destroy_user(user);

	cleanup_srcu_struct(&user->release_barrier);
kref_put(&user->refcount, free_user);

return 0;
}
