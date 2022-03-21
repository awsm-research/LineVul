struct bpf_map *bpf_map_get_with_uref(u32 ufd)
{
struct fd f = fdget(ufd);
struct bpf_map *map;

map = __bpf_map_get(f);
if (IS_ERR(map))
return map;

	bpf_map_inc(map, true);
fdput(f);

return map;
}
