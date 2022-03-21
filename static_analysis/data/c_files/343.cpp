static void skel(const char *homedir, uid_t u, gid_t g) {
char *fname;

// zsh
if (!arg_shell_none && (strcmp(cfg.shell,"/usr/bin/zsh") == 0 || strcmp(cfg.shell,"/bin/zsh") == 0)) {
// copy skel files
if (asprintf(&fname, "%s/.zshrc", homedir) == -1)
errExit("asprintf");
struct stat s;
// don't copy it if we already have the file
if (stat(fname, &s) == 0)
return;
if (stat("/etc/skel/.zshrc", &s) == 0) {
			copy_file("/etc/skel/.zshrc", fname, u, g, 0644);
fs_logger("clone /etc/skel/.zshrc");
}
else {
touch_file_as_user(fname, u, g, 0644);
fs_logger2("touch", fname);
}
free(fname);
}
// csh
else if (!arg_shell_none && strcmp(cfg.shell,"/bin/csh") == 0) {
// copy skel files
if (asprintf(&fname, "%s/.cshrc", homedir) == -1)
errExit("asprintf");
struct stat s;
// don't copy it if we already have the file
if (stat(fname, &s) == 0)
return;
if (stat("/etc/skel/.cshrc", &s) == 0) {
			copy_file("/etc/skel/.cshrc", fname, u, g, 0644);
fs_logger("clone /etc/skel/.cshrc");
}
else {
touch_file_as_user(fname, u, g, 0644);
fs_logger2("touch", fname);
}
free(fname);
}
// bash etc.
else {
// copy skel files
if (asprintf(&fname, "%s/.bashrc", homedir) == -1)
errExit("asprintf");
struct stat s;
// don't copy it if we already have the file
if (stat(fname, &s) == 0)
return;
if (stat("/etc/skel/.bashrc", &s) == 0) {
			copy_file("/etc/skel/.bashrc", fname, u, g, 0644);
fs_logger("clone /etc/skel/.bashrc");
}
free(fname);
}
}
