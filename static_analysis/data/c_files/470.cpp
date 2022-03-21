AirPDcapDecryptWPABroadcastKey(const EAPOL_RSN_KEY *pEAPKey, guint8 *decryption_key, PAIRPDCAP_SEC_ASSOCIATION sa, guint eapol_len)
{
guint8 key_version;
guint8 *key_data;
guint8  *szEncryptedKey;
guint16 key_bytes_len = 0; /* Length of the total key data field */
guint16 key_len;           /* Actual group key length */
static AIRPDCAP_KEY_ITEM dummy_key; /* needed in case AirPDcapRsnaMng() wants the key structure */
AIRPDCAP_SEC_ASSOCIATION *tmp_sa;

/* We skip verifying the MIC of the key. If we were implementing a WPA supplicant we'd want to verify, but for a sniffer it's not needed. */

/* Preparation for decrypting the group key -  determine group key data length */
/* depending on whether the pairwise key is TKIP or AES encryption key */
key_version = AIRPDCAP_EAP_KEY_DESCR_VER(pEAPKey->key_information[1]);
if (key_version == AIRPDCAP_WPA_KEY_VER_NOT_CCMP){
/* TKIP */
key_bytes_len = pntoh16(pEAPKey->key_length);
}else if (key_version == AIRPDCAP_WPA_KEY_VER_AES_CCMP){
/* AES */
key_bytes_len = pntoh16(pEAPKey->key_data_len);

/* AES keys must be at least 128 bits = 16 bytes. */
if (key_bytes_len < 16) {
return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
}
}

    if (key_bytes_len < GROUP_KEY_MIN_LEN || key_bytes_len > eapol_len - sizeof(EAPOL_RSN_KEY)) {
return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
}

/* Encrypted key is in the information element field of the EAPOL key packet */
key_data = (guint8 *)pEAPKey + sizeof(EAPOL_RSN_KEY);
szEncryptedKey = (guint8 *)g_memdup(key_data, key_bytes_len);

DEBUG_DUMP("Encrypted Broadcast key:", szEncryptedKey, key_bytes_len);
DEBUG_DUMP("KeyIV:", pEAPKey->key_iv, 16);
DEBUG_DUMP("decryption_key:", decryption_key, 16);

/* We are rekeying, save old sa */
tmp_sa=(AIRPDCAP_SEC_ASSOCIATION *)g_malloc(sizeof(AIRPDCAP_SEC_ASSOCIATION));
memcpy(tmp_sa, sa, sizeof(AIRPDCAP_SEC_ASSOCIATION));
sa->next=tmp_sa;

/* As we have no concept of the prior association request at this point, we need to deduce the     */
/* group key cipher from the length of the key bytes. In WPA this is straightforward as the        */
/* keybytes just contain the GTK, and the GTK is only in the group handshake, NOT the M3.          */
/* In WPA2 its a little more tricky as the M3 keybytes contain an RSN_IE, but the group handshake  */
/* does not. Also there are other (variable length) items in the keybytes which we need to account */
/* for to determine the true key length, and thus the group cipher.                                */

if (key_version == AIRPDCAP_WPA_KEY_VER_NOT_CCMP){
guint8 new_key[32];
guint8 dummy[256];
/* TKIP key */
/* Per 802.11i, Draft 3.0 spec, section 8.5.2, p. 97, line 4-8, */
/* group key is decrypted using RC4.  Concatenate the IV with the 16 byte EK (PTK+16) to get the decryption key */

rc4_state_struct rc4_state;

/* The WPA group key just contains the GTK bytes so deducing the type is straightforward   */
/* Note - WPA M3 doesn't contain a group key so we'll only be here for the group handshake */
sa->wpa.key_ver = (key_bytes_len >=TKIP_GROUP_KEY_LEN)?AIRPDCAP_WPA_KEY_VER_NOT_CCMP:AIRPDCAP_WPA_KEY_VER_AES_CCMP;

/* Build the full decryption key based on the IV and part of the pairwise key */
memcpy(new_key, pEAPKey->key_iv, 16);
memcpy(new_key+16, decryption_key, 16);
DEBUG_DUMP("FullDecrKey:", new_key, 32);

crypt_rc4_init(&rc4_state, new_key, sizeof(new_key));

/* Do dummy 256 iterations of the RC4 algorithm (per 802.11i, Draft 3.0, p. 97 line 6) */
crypt_rc4(&rc4_state, dummy, 256);
crypt_rc4(&rc4_state, szEncryptedKey, key_bytes_len);

} else if (key_version == AIRPDCAP_WPA_KEY_VER_AES_CCMP){
/* AES CCMP key */

guint8 key_found;
guint8 key_length;
guint16 key_index;
guint8 *decrypted_data;

/* Unwrap the key; the result is key_bytes_len in length */
decrypted_data = AES_unwrap(decryption_key, 16, szEncryptedKey,  key_bytes_len);

/* With WPA2 what we get after Broadcast Key decryption is an actual RSN structure.
The key itself is stored as a GTK KDE
WPA2 IE (1 byte) id = 0xdd, length (1 byte), GTK OUI (4 bytes), key index (1 byte) and 1 reserved byte. Thus we have to
pass pointer to the actual key with 8 bytes offset */

key_found = FALSE;
key_index = 0;

/* Parse Key data until we found GTK KDE */
/* GTK KDE = 00-0F-AC 01 */
while(key_index < (key_bytes_len - 6) && !key_found){
guint8 rsn_id;
guint32 type;

/* Get RSN ID */
rsn_id = decrypted_data[key_index];
type = ((decrypted_data[key_index + 2] << 24) +
(decrypted_data[key_index + 3] << 16) +
(decrypted_data[key_index + 4] << 8) +
(decrypted_data[key_index + 5]));

if (rsn_id == 0xdd && type == 0x000fac01) {
key_found = TRUE;
} else {
key_index += decrypted_data[key_index+1]+2;
}
}

if (key_found){
key_length = decrypted_data[key_index+1] - 6;

if (key_index+8 >= key_bytes_len ||
key_length > key_bytes_len - key_index - 8) {
g_free(decrypted_data);
g_free(szEncryptedKey);
return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
}

/* Skip over the GTK header info, and don't copy past the end of the encrypted data */
memcpy(szEncryptedKey, decrypted_data+key_index+8, key_length);
} else {
g_free(decrypted_data);
g_free(szEncryptedKey);
return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
}

if (key_length == TKIP_GROUP_KEY_LEN)
sa->wpa.key_ver = AIRPDCAP_WPA_KEY_VER_NOT_CCMP;
else
sa->wpa.key_ver = AIRPDCAP_WPA_KEY_VER_AES_CCMP;

g_free(decrypted_data);
}

key_len = (sa->wpa.key_ver==AIRPDCAP_WPA_KEY_VER_NOT_CCMP)?TKIP_GROUP_KEY_LEN:CCMP_GROUP_KEY_LEN;
if (key_len > key_bytes_len) {
/* the key required for this protocol is longer than the key that we just calculated */
g_free(szEncryptedKey);
return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
}

/* Decrypted key is now in szEncryptedKey with len of key_len */
DEBUG_DUMP("Broadcast key:", szEncryptedKey, key_len);

/* Load the proper key material info into the SA */
sa->key = &dummy_key;  /* we just need key to be not null because it is checked in AirPDcapRsnaMng().  The WPA key materials are actually in the .wpa structure */
sa->validKey = TRUE;

/* Since this is a GTK and its size is only 32 bytes (vs. the 64 byte size of a PTK), we fake it and put it in at a 32-byte offset so the  */
/* AirPDcapRsnaMng() function will extract the right piece of the GTK for decryption. (The first 16 bytes of the GTK are used for decryption.) */
memset(sa->wpa.ptk, 0, sizeof(sa->wpa.ptk));
memcpy(sa->wpa.ptk+32, szEncryptedKey, key_len);
g_free(szEncryptedKey);
return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
}
