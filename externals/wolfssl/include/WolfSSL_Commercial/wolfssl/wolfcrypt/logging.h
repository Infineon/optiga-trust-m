/* logging.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */



/* submitted by eof */


#ifndef WOLFSSL_LOGGING_H
#define WOLFSSL_LOGGING_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef __cplusplus
    extern "C" {
#endif


enum  CYA_Log_Levels {
    ERROR_LOG = 0,
    INFO_LOG,
    ENTER_LOG,
    LEAVE_LOG,
    OTHER_LOG
};

typedef void (*wolfSSL_Logging_cb)(const int logLevel,
                                  const char *const logMessage);

WOLFSSL_API int wolfSSL_SetLoggingCb(wolfSSL_Logging_cb log_function);

#if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
    WOLFSSL_LOCAL int wc_LoggingInit(void);
    WOLFSSL_LOCAL int wc_LoggingCleanup(void);
    WOLFSSL_LOCAL int wc_AddErrorNode(int error, int line, char* buf,
            char* file);
    WOLFSSL_LOCAL int wc_PeekErrorNode(int index, const char **file,
            const char **reason, int *line);
    WOLFSSL_API   int wc_SetLoggingHeap(void* h);
    #if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
        WOLFSSL_API   void wc_ERR_print_errors_fp(FILE* fp);
    #endif
#endif /* defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE) */

#ifdef DEBUG_WOLFSSL
    /* a is prepended to m and b is appended, creating a log msg a + m + b */
    #define WOLFSSL_LOG_CAT(a, m, b) #a " " m " "  #b

    void WOLFSSL_ENTER(const char* msg);
    void WOLFSSL_LEAVE(const char* msg, int ret);
    #define WOLFSSL_STUB(m) \
        WOLFSSL_MSG(WOLFSSL_LOG_CAT(wolfSSL Stub, m, not implemented))

#if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
    void WOLFSSL_ERROR_LINE(int err, const char* func, unsigned int line,
            const char* file, void* ctx);
    #define WOLFSSL_ERROR(x) WOLFSSL_ERROR_LINE((x), __func__, __LINE__, __FILE__,NULL)
#else
    void WOLFSSL_ERROR(int);
#endif
    void WOLFSSL_MSG(const char* msg);
    void WOLFSSL_BUFFER(byte* buffer, word32 length);

#else /* DEBUG_WOLFSSL   */

    #define WOLFSSL_ENTER(m)
    #define WOLFSSL_LEAVE(m, r)
    #define WOLFSSL_STUB(m)

    #define WOLFSSL_ERROR(e)
    #define WOLFSSL_MSG(m)
    #define WOLFSSL_BUFFER(b, l)

#endif /* DEBUG_WOLFSSL  */

#ifdef __cplusplus
}
#endif
#endif /* WOLFSSL_LOGGING_H */

