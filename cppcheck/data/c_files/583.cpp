static NOINLINE char *xmalloc_optname_optval(uint8_t *option, const struct dhcp_optflag *optflag, const char *opt_name)
{
unsigned upper_length;
int len, type, optlen;
char *dest, *ret;

/* option points to OPT_DATA, need to go back to get OPT_LEN */
len = option[-OPT_DATA + OPT_LEN];

type = optflag->flags & OPTION_TYPE_MASK;
optlen = dhcp_option_lengths[type];
upper_length = len_of_option_as_string[type]
		* ((unsigned)(len + optlen - 1) / (unsigned)optlen);

dest = ret = xmalloc(upper_length + strlen(opt_name) + 2);
dest += sprintf(ret, "%s=", opt_name);

while (len >= optlen) {
switch (type) {
case OPTION_IP:
case OPTION_IP_PAIR:
dest += sprint_nip(dest, "", option);
if (type == OPTION_IP)
break;
dest += sprint_nip(dest, "/", option + 4);
break;
//		case OPTION_BOOLEAN:
//			dest += sprintf(dest, *option ? "yes" : "no");
//			break;
case OPTION_U8:
dest += sprintf(dest, "%u", *option);
break;
//		case OPTION_S16:
case OPTION_U16: {
uint16_t val_u16;
move_from_unaligned16(val_u16, option);
dest += sprintf(dest, "%u", ntohs(val_u16));
break;
}
case OPTION_S32:
case OPTION_U32: {
uint32_t val_u32;
move_from_unaligned32(val_u32, option);
dest += sprintf(dest, type == OPTION_U32 ? "%lu" : "%ld", (unsigned long) ntohl(val_u32));
break;
}
/* Note: options which use 'return' instead of 'break'
* (for example, OPTION_STRING) skip the code which handles
* the case of list of options.
*/
case OPTION_STRING:
case OPTION_STRING_HOST:
memcpy(dest, option, len);
dest[len] = '\0';
if (type == OPTION_STRING_HOST && !good_hostname(dest))
safe_strncpy(dest, "bad", len);
return ret;
case OPTION_STATIC_ROUTES: {
/* Option binary format:
* mask [one byte, 0..32]
* ip [big endian, 0..4 bytes depending on mask]
* router [big endian, 4 bytes]
* may be repeated
*
* We convert it to a string "IP/MASK ROUTER IP2/MASK2 ROUTER2"
*/
const char *pfx = "";

while (len >= 1 + 4) { /* mask + 0-byte ip + router */
uint32_t nip;
uint8_t *p;
unsigned mask;
int bytes;

mask = *option++;
if (mask > 32)
break;
len--;

nip = 0;
p = (void*) &nip;
bytes = (mask + 7) / 8; /* 0 -> 0, 1..8 -> 1, 9..16 -> 2 etc */
while (--bytes >= 0) {
*p++ = *option++;
len--;
}
if (len < 4)
break;

/* print ip/mask */
dest += sprint_nip(dest, pfx, (void*) &nip);
pfx = " ";
dest += sprintf(dest, "/%u ", mask);
/* print router */
dest += sprint_nip(dest, "", option);
option += 4;
len -= 4;
}

return ret;
}
case OPTION_6RD:
/* Option binary format (see RFC 5969):
*  0                   1                   2                   3
*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* |  OPTION_6RD   | option-length |  IPv4MaskLen  |  6rdPrefixLen |
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* |                           6rdPrefix                           |
* ...                        (16 octets)                        ...
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* ...                   6rdBRIPv4Address(es)                    ...
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* We convert it to a string
* "IPv4MaskLen 6rdPrefixLen 6rdPrefix 6rdBRIPv4Address..."
*
* Sanity check: ensure that our length is at least 22 bytes, that
* IPv4MaskLen <= 32,
* 6rdPrefixLen <= 128,
* 6rdPrefixLen + (32 - IPv4MaskLen) <= 128
* (2nd condition need no check - it follows from 1st and 3rd).
* Else, return envvar with empty value ("optname=")
*/
if (len >= (1 + 1 + 16 + 4)
&& option[0] <= 32
&& (option[1] + 32 - option[0]) <= 128
) {
/* IPv4MaskLen */
dest += sprintf(dest, "%u ", *option++);
/* 6rdPrefixLen */
dest += sprintf(dest, "%u ", *option++);
/* 6rdPrefix */
dest += sprint_nip6(dest, /* "", */ option);
option += 16;
len -= 1 + 1 + 16 + 4;
/* "+ 4" above corresponds to the length of IPv4 addr
* we consume in the loop below */
while (1) {
/* 6rdBRIPv4Address(es) */
dest += sprint_nip(dest, " ", option);
option += 4;
len -= 4; /* do we have yet another 4+ bytes? */
if (len < 0)
break; /* no */
}
}

return ret;
#if ENABLE_FEATURE_UDHCP_RFC3397
case OPTION_DNS_STRING:
/* unpack option into dest; use ret for prefix (i.e., "optname=") */
dest = dname_dec(option, len, ret);
if (dest) {
free(ret);
return dest;
}
/* error. return "optname=" string */
return ret;
case OPTION_SIP_SERVERS:
/* Option binary format:
* type: byte
* type=0: domain names, dns-compressed
* type=1: IP addrs
*/
option++;
len--;
if (option[-1] == 0) {
dest = dname_dec(option, len, ret);
if (dest) {
free(ret);
return dest;
}
} else
if (option[-1] == 1) {
const char *pfx = "";
while (1) {
len -= 4;
if (len < 0)
break;
dest += sprint_nip(dest, pfx, option);
pfx = " ";
option += 4;
}
}
return ret;
#endif
} /* switch */

/* If we are here, try to format any remaining data
* in the option as another, similarly-formatted option
*/
option += optlen;
len -= optlen;
// TODO: it can be a list only if (optflag->flags & OPTION_LIST).
// Should we bail out/warn if we see multi-ip option which is
// not allowed to be such (for example, DHCP_BROADCAST)? -
if (len < optlen /* || !(optflag->flags & OPTION_LIST) */)
break;
*dest++ = ' ';
*dest = '\0';
} /* while */

return ret;
}
