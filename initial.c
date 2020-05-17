#include "initial.h"

// 創建 SSL context
SSL_CTX *create_context(int mode)
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    
    method = TLSv1_2_server_method();    // create 的方法

    ctx = SSL_CTX_new(method);
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}

// 配置 SSL context
void configure_context(SSL_CTX *ctx, const char *cert, const char *key)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);  // 選擇橢圓曲線

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }
}