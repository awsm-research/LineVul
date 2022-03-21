static void nfs4_open_confirm_release(void *calldata)
{
struct nfs4_opendata *data = calldata;
struct nfs4_state *state = NULL;

/* If this request hasn't been cancelled, do nothing */
if (data->cancelled == 0)
goto out_free;
/* In case of error, no cleanup! */
if (!data->rpc_done)
goto out_free;
state = nfs4_opendata_to_nfs4_state(data);
if (!IS_ERR(state))
		nfs4_close_state(&data->path, state, data->o_arg.open_flags);
out_free:
nfs4_opendata_put(data);
}
