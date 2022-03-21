static void copy_asoundrc(void) {
// copy XAUTHORITY_FILE in the new home directory
char *src = RUN_ASOUNDRC_FILE ;
char *dest;
if (asprintf(&dest, "%s/.asoundrc", cfg.homedir) == -1)
errExit("asprintf");
// if destination is a symbolic link, exit the sandbox!!!
if (is_link(dest)) {
fprintf(stderr, "Error: %s is a symbolic link\n", dest);
exit(1);
}

	pid_t child = fork();
	if (child < 0)
		errExit("fork");
	if (child == 0) {
		// drop privileges
		drop_privs(0);
		// copy, set permissions and ownership
		int rv = copy_file(src, dest);
		if (rv)
			fprintf(stderr, "Warning: cannot transfer .asoundrc in private home directory\n");
		else {
			fs_logger2("clone", dest);
		}
		_exit(0);
	}
	// wait for the child to finish
	waitpid(child, NULL, 0);
	// set permissions and ownership
	if (chown(dest, getuid(), getgid()) < 0)
		errExit("chown");
	if (chmod(dest, S_IRUSR | S_IWUSR) < 0)
		errExit("chmod");

// delete the temporary file
unlink(src);
}
