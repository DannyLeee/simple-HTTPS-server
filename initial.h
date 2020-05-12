#include <openssl/ssl.h>
#include <openssl/err.h>

// set the ca path here
#define CA_CERT "./certificate/ca.crt"

// set the certificate path here
// right server certificate
#define HOST_CERT "./certificate/host.crt"
#define HOST_KEY "./certificate/host.key"

// wrong certificate
#define WRONG_CERT "wrong.crt"
#define WRONG_KEY "wrong.key"

SSL_CTX* create_context(int mode);
void configure_context(SSL_CTX *ctx, const char *cert, const char *key);
void ShowCerts(SSL* ssl, int mode);