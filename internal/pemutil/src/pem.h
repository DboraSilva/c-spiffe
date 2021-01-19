#ifndef __INCLUDE_INTERNAL_PEMUTIL_PEM_H__
#define __INCLUDE_INTERNAL_PEMUTIL_PEM_H__

#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include "../../../utils/src/util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pemutil_Block
{
    string_t type;
    // map_string_string *headers;
    byte *bytes;
} pemutil_Block;

void* parseBlock(BIO *bio_mem, 
                    const char *type, 
                    err_t *err);

void** parseBlocks(const byte *pem_byte, 
                    const char *type, 
                    err_t *err);

X509** pemutil_ParseCertificates(const byte *bytes, err_t *err);
EVP_PKEY* pemutil_ParsePrivateKey(const byte *bytes, 
                                    err_t *err);
byte* pemutil_EncodePrivateKey(EVP_PKEY *pkey, 
                                int *bytes_len, 
                                err_t *err);
byte** pemutil_EncodeCertificates(X509 **certs);

/*
func parseBlocks(blocksBytes []byte, expectedType string) ([]interface{}, error)
func parseBlock(pemBytes []byte, pemType string) (interface{}, []byte, error)
*/

#ifdef __cplusplus
}
#endif

#endif
