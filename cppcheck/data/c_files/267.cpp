mark_source_chains(const struct xt_table_info *newinfo,
unsigned int valid_hooks, void *entry0)
{
unsigned int hook;

/* No recursion; use packet counter to save back ptrs (reset
to 0 as we leave), and comefrom to save source hook bitmask */
for (hook = 0; hook < NF_INET_NUMHOOKS; hook++) {
unsigned int pos = newinfo->hook_entry[hook];
struct ipt_entry *e = (struct ipt_entry *)(entry0 + pos);

if (!(valid_hooks & (1 << hook)))
continue;

/* Set initial back pointer. */
e->counters.pcnt = pos;

for (;;) {
const struct xt_standard_target *t
= (void *)ipt_get_target_c(e);
int visited = e->comefrom & (1 << hook);

if (e->comefrom & (1 << NF_INET_NUMHOOKS)) {
pr_err("iptables: loop hook %u pos %u %08X.\n",
hook, pos, e->comefrom);
return 0;
}
e->comefrom |= ((1 << hook) | (1 << NF_INET_NUMHOOKS));

/* Unconditional return/END. */
			if ((e->target_offset == sizeof(struct ipt_entry) &&
(strcmp(t->target.u.user.name,
XT_STANDARD_TARGET) == 0) &&
			     t->verdict < 0 && unconditional(&e->ip)) ||
			    visited) {
unsigned int oldpos, size;

if ((strcmp(t->target.u.user.name,
XT_STANDARD_TARGET) == 0) &&
t->verdict < -NF_MAX_VERDICT - 1) {
duprintf("mark_source_chains: bad "
"negative verdict (%i)\n",
t->verdict);
return 0;
}

/* Return: backtrack through the last
big jump. */
do {
e->comefrom ^= (1<<NF_INET_NUMHOOKS);
#ifdef DEBUG_IP_FIREWALL_USER
if (e->comefrom
& (1 << NF_INET_NUMHOOKS)) {
duprintf("Back unset "
"on hook %u "
"rule %u\n",
hook, pos);
}
#endif
oldpos = pos;
pos = e->counters.pcnt;
e->counters.pcnt = 0;

/* We're at the start. */
if (pos == oldpos)
goto next;

e = (struct ipt_entry *)
(entry0 + pos);
} while (oldpos == pos + e->next_offset);

/* Move along one */
size = e->next_offset;
e = (struct ipt_entry *)
(entry0 + pos + size);
e->counters.pcnt = pos;
pos += size;
} else {
int newpos = t->verdict;

if (strcmp(t->target.u.user.name,
XT_STANDARD_TARGET) == 0 &&
newpos >= 0) {
if (newpos > newinfo->size -
sizeof(struct ipt_entry)) {
duprintf("mark_source_chains: "
"bad verdict (%i)\n",
newpos);
return 0;
}
/* This a jump; chase it. */
duprintf("Jump rule %u -> %u\n",
pos, newpos);
} else {
/* ... this is a fallthru */
newpos = pos + e->next_offset;
}
e = (struct ipt_entry *)
(entry0 + newpos);
e->counters.pcnt = pos;
pos = newpos;
}
}
next:
duprintf("Finished chain %u\n", hook);
}
return 1;
}
