static int hash_recvmsg(struct kiocb *unused, struct socket *sock,
struct msghdr *msg, size_t len, int flags)
{
struct sock *sk = sock->sk;
struct alg_sock *ask = alg_sk(sk);
struct hash_ctx *ctx = ask->private;
unsigned ds = crypto_ahash_digestsize(crypto_ahash_reqtfm(&ctx->req));
int err;

if (len > ds)
len = ds;
else if (len < ds)
msg->msg_flags |= MSG_TRUNC;

	msg->msg_namelen = 0;
lock_sock(sk);
if (ctx->more) {
ctx->more = 0;
ahash_request_set_crypt(&ctx->req, NULL, ctx->result, 0);
err = af_alg_wait_for_completion(crypto_ahash_final(&ctx->req),
&ctx->completion);
if (err)
goto unlock;
}

err = memcpy_toiovec(msg->msg_iov, ctx->result, len);

unlock:
release_sock(sk);

return err ?: len;
}
