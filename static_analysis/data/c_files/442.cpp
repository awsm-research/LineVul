ossl_cipher_initialize(VALUE self, VALUE str)
{
EVP_CIPHER_CTX *ctx;
const EVP_CIPHER *cipher;
char *name;
    unsigned char dummy_key[EVP_MAX_KEY_LENGTH] = { 0 };

name = StringValueCStr(str);
GetCipherInit(self, ctx);
if (ctx) {
ossl_raise(rb_eRuntimeError, "Cipher already inititalized!");
}
AllocCipher(self, ctx);
if (!(cipher = EVP_get_cipherbyname(name))) {
ossl_raise(rb_eRuntimeError, "unsupported cipher algorithm (%"PRIsVALUE")", str);
}
    /*
     * EVP_CipherInit_ex() allows to specify NULL to key and IV, however some
     * ciphers don't handle well (OpenSSL's bug). [Bug #2768]
     *
     * The EVP which has EVP_CIPH_RAND_KEY flag (such as DES3) allows
     * uninitialized key, but other EVPs (such as AES) does not allow it.
     * Calling EVP_CipherUpdate() without initializing key causes SEGV so we
     * set the data filled with "\0" as the key by default.
     *
    if (EVP_CipherInit_ex(ctx, cipher, NULL, dummy_key, NULL, -1) != 1)
ossl_raise(eCipherError, NULL);

return self;
}
