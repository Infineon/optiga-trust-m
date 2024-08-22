/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_shared_mutex.h
 *
 * \brief   This file provides the prototype declarations shared memory acquire
 * and release.
 *
 * \ingroup  grPAL
 * @{
 */
#ifndef _PAL_SHARED_MEM_MUTEX_H_
#define _PAL_SHARED_MEM_MUTEX_H_

#include <pthread.h>  // pthread_mutex_t, pthread_mutexattr_t,
// pthread_mutexattr_init, pthread_mutexattr_setpshared,
// pthread_mutex_init, pthread_mutex_destroy
#include "pal.h"
// Structure of a shared mutex.
typedef struct shared_mutex_t {
    pthread_mutex_t *ptr;  // Pointer to the pthread mutex and
        // shared memory segment.
    int shm_fd;  // Descriptor of shared memory object.
    char *name;  // Name of the mutex and associated
        // shared memory object.
    int created;  // Equals 1 (true) if initialization
        // of this structure caused creation
        // of a new shared mutex.
        // Equals 0 (false) if this mutex was
        // just retrieved from shared memory.
    pid_t *pid;  // PID of the process that previously seized the mutex

} shared_mutex_t;
typedef struct trustm_mutex_t {
    pthread_mutex_t mutex;
    pid_t pid;
} trustm_mutex_t;
#define EMPTY_PID 0x55AA55AA

#define DEBUG_TRUSTM_MUTEX_L1 0
#define DEBUG_TRUSTM_MUTEX_L2 0

#if DEBUG_TRUSTM_MUTEX_L1 == 1
#define TRUSTM_MUTEX_DBGFN1(x, ...) \
    fprintf( \
        stdout, \
        "%d:%s:%d %s: " x "\n", \
        getpid(), \
        __FILE__, \
        __LINE__, \
        __FUNCTION__, \
        ##__VA_ARGS__ \
    )
#elif DEBUG_TRUSTM_MUTEX_L1 == 2

#define TRUSTM_MUTEX_DBGFN1(x, ...) \
    Log("%d:%s:%d %s: " x "\n", getpid(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define TRUSTM_MUTEX_DBGFN1(x, ...)
#endif

#if DEBUG_TRUSTM_MUTEX_L2 == 1
#define TRUSTM_MUTEX_DBGFN2(x, ...) \
    fprintf( \
        stdout, \
        "%d:%s:%d %s: " x "\n", \
        getpid(), \
        __FILE__, \
        __LINE__, \
        __FUNCTION__, \
        ##__VA_ARGS__ \
    )
#elif DEBUG_TRUSTM_MUTEX_L2 == 2
#define TRUSTM_MUTEX_DBGFN2(x, ...) \
    Log("%d:%s:%d %s: " x "\n", getpid(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define TRUSTM_MUTEX_DBGFN2(x, ...)
#endif

pal_status_t pal_shm_mutex_acquire(shared_mutex_t *shm_mutex, const char *mutex_name);
void pal_shm_mutex_release(shared_mutex_t *shm_mutex);

#endif
