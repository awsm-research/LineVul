static void ext2_put_super (struct super_block * sb)
{
int db_count;
int i;
struct ext2_sb_info *sbi = EXT2_SB(sb);

dquot_disable(sb, -1, DQUOT_USAGE_ENABLED | DQUOT_LIMITS_ENABLED);

	ext2_xattr_put_super(sb);
if (!(sb->s_flags & MS_RDONLY)) {
struct ext2_super_block *es = sbi->s_es;

spin_lock(&sbi->s_lock);
es->s_state = cpu_to_le16(sbi->s_mount_state);
spin_unlock(&sbi->s_lock);
ext2_sync_super(sb, es, 1);
}
db_count = sbi->s_gdb_count;
for (i = 0; i < db_count; i++)
if (sbi->s_group_desc[i])
brelse (sbi->s_group_desc[i]);
kfree(sbi->s_group_desc);
kfree(sbi->s_debts);
percpu_counter_destroy(&sbi->s_freeblocks_counter);
percpu_counter_destroy(&sbi->s_freeinodes_counter);
percpu_counter_destroy(&sbi->s_dirs_counter);
brelse (sbi->s_sbh);
sb->s_fs_info = NULL;
kfree(sbi->s_blockgroup_lock);
kfree(sbi);
}
