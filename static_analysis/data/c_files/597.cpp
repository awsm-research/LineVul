void set_fat(DOS_FS * fs, uint32_t cluster, int32_t new)
{
unsigned char *data = NULL;
int size;
loff_t offs;

if (new == -1)
new = FAT_EOF(fs);
else if ((long)new == -2)
new = FAT_BAD(fs);
switch (fs->fat_bits) {
case 12:
data = fs->fat + cluster * 3 / 2;
offs = fs->fat_start + cluster * 3 / 2;
if (cluster & 1) {
FAT_ENTRY prevEntry;
get_fat(&prevEntry, fs->fat, cluster - 1, fs);
data[0] = ((new & 0xf) << 4) | (prevEntry.value >> 8);
data[1] = new >> 4;
} else {
FAT_ENTRY subseqEntry;
	    if (cluster != fs->clusters - 1)
get_fat(&subseqEntry, fs->fat, cluster + 1, fs);
else
subseqEntry.value = 0;
data[0] = new & 0xff;
data[1] = (new >> 8) | ((0xff & subseqEntry.value) << 4);
}
size = 2;
break;
case 16:
data = fs->fat + cluster * 2;
offs = fs->fat_start + cluster * 2;
*(unsigned short *)data = htole16(new);
size = 2;
break;
case 32:
{
FAT_ENTRY curEntry;
get_fat(&curEntry, fs->fat, cluster, fs);

data = fs->fat + cluster * 4;
offs = fs->fat_start + cluster * 4;
/* According to M$, the high 4 bits of a FAT32 entry are reserved and
* are not part of the cluster number. So we never touch them. */
*(uint32_t *)data = htole32((new & 0xfffffff) |
(curEntry.reserved << 28));
size = 4;
}
break;
default:
die("Bad FAT entry size: %d bits.", fs->fat_bits);
}
fs_write(offs, size, data);
if (fs->nfats > 1) {
fs_write(offs + fs->fat_size, size, data);
}
}
