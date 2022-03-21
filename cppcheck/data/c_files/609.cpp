long ssl_get_algorithm2(SSL *s)
{
long alg2 = s->s3->tmp.new_cipher->algorithm2;
       if (TLS1_get_version(s) >= TLS1_2_VERSION &&
alg2 == (SSL_HANDSHAKE_MAC_DEFAULT|TLS1_PRF))
return SSL_HANDSHAKE_MAC_SHA256 | TLS1_PRF_SHA256;
return alg2;
}
