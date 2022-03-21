parse_netscreen_hex_dump(FILE_T fh, int pkt_len, const char *cap_int,
    const char *cap_dst, struct wtap_pkthdr *phdr, Buffer* buf,
    int *err, gchar **err_info)
{
	guint8	*pd;
	gchar	line[NETSCREEN_LINE_LENGTH];
	gchar	*p;
	int	n, i = 0, offset = 0;
	gchar	dststr[13];
/* Make sure we have enough room for the packet */
	ws_buffer_assure_space(buf, NETSCREEN_MAX_PACKET_LEN);
pd = ws_buffer_start_ptr(buf);

while(1) {

/* The last packet is not delimited by an empty line, but by EOF
* So accept EOF as a valid delimiter too
*/
if (file_gets(line, NETSCREEN_LINE_LENGTH, fh) == NULL) {
break;
}

/*
* Skip blanks.
* The number of blanks is not fixed - for wireless
* interfaces, there may be 14 extra spaces before
* the hex data.
*/
for (p = &line[0]; g_ascii_isspace(*p); p++)
;
/* packets are delimited with empty lines */
if (*p == '\0') {
break;
}

n = parse_single_hex_dump_line(p, pd, offset);

/* the smallest packet has a length of 6 bytes, if
* the first hex-data is less then check whether
* it is a info-line and act accordingly
*/
if (offset == 0 && n < 6) {
if (info_line(line)) {
if (++i <= NETSCREEN_MAX_INFOLINES) {
continue;
}
} else {
*err = WTAP_ERR_BAD_FILE;
*err_info = g_strdup("netscreen: cannot parse hex-data");
return FALSE;
}
}

/* If there is no more data and the line was not empty,
* then there must be an error in the file
*/
		if(n == -1) {
*err = WTAP_ERR_BAD_FILE;
*err_info = g_strdup("netscreen: cannot parse hex-data");
return FALSE;
}

/* Adjust the offset to the data that was just added to the buffer */
offset += n;

/* If there was more hex-data than was announced in the len=x
* header, then then there must be an error in the file
*/
		if(offset > pkt_len) {
*err = WTAP_ERR_BAD_FILE;
*err_info = g_strdup("netscreen: too much hex-data");
return FALSE;
}
}

/*
* Determine the encapsulation type, based on the
* first 4 characters of the interface name
*
* XXX	convert this to a 'case' structure when adding more
*	(non-ethernet) interfacetypes
*/
if (strncmp(cap_int, "adsl", 4) == 0) {
/* The ADSL interface can be bridged with or without
* PPP encapsulation. Check whether the first six bytes
* of the hex data are the same as the destination mac
* address in the header. If they are, assume ethernet
* LinkLayer or else PPP
*/
g_snprintf(dststr, 13, "%02x%02x%02x%02x%02x%02x",
pd[0], pd[1], pd[2], pd[3], pd[4], pd[5]);
if (strncmp(dststr, cap_dst, 12) == 0)
phdr->pkt_encap = WTAP_ENCAP_ETHERNET;
else
phdr->pkt_encap = WTAP_ENCAP_PPP;
}
else if (strncmp(cap_int, "seri", 4) == 0)
phdr->pkt_encap = WTAP_ENCAP_PPP;
else
phdr->pkt_encap = WTAP_ENCAP_ETHERNET;

phdr->caplen = offset;

return TRUE;
}
