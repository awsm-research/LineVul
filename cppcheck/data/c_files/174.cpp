static int ps_files_valid_key(const char *key)
{
       size_t len;
       const char *p;
       char c;
       int ret = 1;
       for (p = key; (c = *p); p++) {
               /* valid characters are a..z,A..Z,0..9 *
               if (!((c >= 'a' && c <= 'z')
                               || (c >= 'A' && c <= 'Z')
                               || (c >= '0' && c <= '9')
                               || c == ','
                               || c == '-')) {
                       ret = 0;
                       break;
               }
       }
       len = p - key;
       /* Somewhat arbitrary length limit here, but should be way more than
          anyone needs and avoids file-level warnings later on if we exceed MAX_PATH *
       if (len == 0 || len > 128) {
               ret = 0;
       }
       return ret;
}
