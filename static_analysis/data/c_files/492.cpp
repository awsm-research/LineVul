static int crypto_ccm_auth(struct aead_request *req, struct scatterlist *plain,
unsigned int cryptlen)
{
struct crypto_ccm_req_priv_ctx *pctx = crypto_ccm_reqctx(req);
struct crypto_aead *aead = crypto_aead_reqtfm(req);
struct crypto_ccm_ctx *ctx = crypto_aead_ctx(aead);
AHASH_REQUEST_ON_STACK(ahreq, ctx->mac);
unsigned int assoclen = req->assoclen;
struct scatterlist sg[3];
	u8 odata[16];
	u8 idata[16];
int ilen, err;

/* format control data for input */
err = format_input(odata, req, cryptlen);
if (err)
goto out;

sg_init_table(sg, 3);
sg_set_buf(&sg[0], odata, 16);

/* format associated data and compute into mac */
if (assoclen) {
ilen = format_adata(idata, assoclen);
sg_set_buf(&sg[1], idata, ilen);
sg_chain(sg, 3, req->src);
} else {
ilen = 0;
sg_chain(sg, 2, req->src);
}

ahash_request_set_tfm(ahreq, ctx->mac);
ahash_request_set_callback(ahreq, pctx->flags, NULL, NULL);
ahash_request_set_crypt(ahreq, sg, NULL, assoclen + ilen + 16);
err = crypto_ahash_init(ahreq);
if (err)
goto out;
err = crypto_ahash_update(ahreq);
if (err)
goto out;

/* we need to pad the MAC input to a round multiple of the block size */
ilen = 16 - (assoclen + ilen) % 16;
if (ilen < 16) {
memset(idata, 0, ilen);
sg_init_table(sg, 2);
sg_set_buf(&sg[0], idata, ilen);
if (plain)
sg_chain(sg, 2, plain);
plain = sg;
cryptlen += ilen;
}

ahash_request_set_crypt(ahreq, plain, pctx->odata, cryptlen);
err = crypto_ahash_finup(ahreq);
out:
return err;
}
