l2tp_accm_print(netdissect_options *ndo, const u_char *dat)
{
const uint16_t *ptr = (const uint16_t *)dat;
uint16_t val_h, val_l;

ptr++;		/* skip "Reserved" */

	val_h = EXTRACT_16BITS(ptr); ptr++;
	val_l = EXTRACT_16BITS(ptr); ptr++;
ND_PRINT((ndo, "send=%08x ", (val_h<<16) + val_l));

val_h = EXTRACT_16BITS(ptr); ptr++;
val_l = EXTRACT_16BITS(ptr); ptr++;
ND_PRINT((ndo, "recv=%08x ", (val_h<<16) + val_l));
}
