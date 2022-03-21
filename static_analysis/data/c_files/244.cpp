int main(int argc, char *argv[])
{
int ret;
struct lxc_lock *lock;

lock = lxc_newlock(NULL, NULL);
if (!lock) {
fprintf(stderr, "%d: failed to get unnamed lock\n", __LINE__);
exit(1);
}
ret = lxclock(lock, 0);
if (ret) {
fprintf(stderr, "%d: failed to take unnamed lock (%d)\n", __LINE__, ret);
exit(1);
}

ret = lxcunlock(lock);
if (ret) {
fprintf(stderr, "%d: failed to put unnamed lock (%d)\n", __LINE__, ret);
exit(1);
}
lxc_putlock(lock);

lock = lxc_newlock("/var/lib/lxc", mycontainername);
if (!lock) {
fprintf(stderr, "%d: failed to get lock\n", __LINE__);
exit(1);
}
struct stat sb;
	char *pathname = RUNTIME_PATH "/lock/lxc/var/lib/lxc/";
ret = stat(pathname, &sb);
if (ret != 0) {
fprintf(stderr, "%d: filename %s not created\n", __LINE__,
pathname);
exit(1);
}
lxc_putlock(lock);

test_two_locks();

fprintf(stderr, "all tests passed\n");

exit(ret);
}
