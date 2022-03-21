static void update_open_stateflags(struct nfs4_state *state, mode_t open_flags)
{
	switch (open_flags) {
case FMODE_WRITE:
state->n_wronly++;
break;
case FMODE_READ:
state->n_rdonly++;
break;
case FMODE_READ|FMODE_WRITE:
state->n_rdwr++;
}
	nfs4_state_set_mode_locked(state, state->state | open_flags);
}
