static unsigned int seedsize(struct crypto_alg *alg)
{
struct rng_alg *ralg = container_of(alg, struct rng_alg, base);

	return alg->cra_rng.rng_make_random ?
	       alg->cra_rng.seedsize : ralg->seedsize;
}
