#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

//lint --e{46} suppress "Suppress internal.h in Keil as it is from third part library"
#include <wolfssl/internal.h> 

#include <wolfssl/wolfcrypt/settings.h>

int PRF(byte* digest, word32 digLen, const byte* secret, word32 secLen,
            const byte* label, word32 labLen, const byte* seed, word32 seedLen,
            int useAtLeastSha256, int hash_type);
