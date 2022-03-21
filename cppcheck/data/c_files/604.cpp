static int inotify_release(struct inode *ignored, struct file *file)
{
struct fsnotify_group *group = file->private_data;
	struct user_struct *user = group->inotify_data.user;

pr_debug("%s: group=%p\n", __func__, group);

fsnotify_clear_marks_by_group(group);

/* free this group, matching get was inotify_init->fsnotify_obtain_group */
fsnotify_put_group(group);

	atomic_dec(&user->inotify_devs);
return 0;
}
