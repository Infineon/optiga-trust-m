/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_shared_mutex.c
 *
 * \brief   This file implements the platform abstraction layer APIs for shared
 * memory mutex.
 *
 * \ingroup  grPAL
 * @{
 */

#include "include/pal_shared_mutex.h"

#include <errno.h>  // errno, ENOENT
#include <fcntl.h>  // O_RDWR, O_CREATE
#include <linux/limits.h>  // NAME_MAX
#include <sys/mman.h>  // shm_open, shm_unlink, mmap, munmap,
// PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED
#include <stdio.h>  // perror
#include <stdlib.h>  // malloc, free
#include <string.h>  // strcpy
#include <unistd.h>  // ftruncate, close

pthread_mutex_t shm_lock;

static shared_mutex_t shared_mutex_init(const char *name) {
    shared_mutex_t mutex = {NULL, 0, NULL, 0, NULL};
    trustm_mutex_t *addr;
    trustm_mutex_t *mutex_ptr;

    // Open existing shared memory object, or create one.
    // Two separate calls are needed here, to mark fact of creation
    // for later initialization of pthread mutex.
    TRUSTM_MUTEX_DBGFN2(">");
    TRUSTM_MUTEX_DBGFN2("pthread lock");
    pthread_mutex_lock(&shm_lock);
    TRUSTM_MUTEX_DBGFN2("pthread lock successfully");
    mutex.shm_fd = shm_open(name, O_RDWR, 0660);
    if (mutex.shm_fd == -1 && errno == ENOENT) {
        mutex.shm_fd = shm_open(name, O_RDWR | O_CREAT, 0660);
        mutex.created = 1;
        TRUSTM_MUTEX_DBGFN2("create new shm");
    }
    TRUSTM_MUTEX_DBGFN2("pthread unlock");
    pthread_mutex_unlock(&shm_lock);
    TRUSTM_MUTEX_DBGFN2("pthread unlock successfully");
    if (mutex.shm_fd == -1) {
        perror("shm_open");
        return mutex;
    }
    TRUSTM_MUTEX_DBGFN2("truncate shm ");
    // Truncate shared memory segment so it would contain
    // trustm_mutex_t.
    if (ftruncate(mutex.shm_fd, sizeof(trustm_mutex_t)) != 0) {
        perror("ftruncate");
        return mutex;
    }

    // Map pthread mutex into the shared memory.
    addr = mmap(NULL, sizeof(trustm_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, mutex.shm_fd, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        return mutex;
    }
    mutex_ptr = addr;

    // If shared memory was just initialized -
    // initialize the mutex as well.

    if (mutex.created) {
        pthread_mutexattr_t attr;
        if (pthread_mutexattr_init(&attr)) {
            perror("pthread_mutexattr_init");
            return mutex;
        }
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) {
            perror("pthread_mutexattr_setpshared");
            return mutex;
        }

        if (pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST)) {
            perror("pthread_mutexattr_setrobust");
            return mutex;
        }

        if (pthread_mutex_init(&mutex_ptr->mutex, &attr)) {
            perror("pthread_mutex_init");
            return mutex;
        }
    }
    TRUSTM_MUTEX_DBGFN2("-----> mutes ptr:%x", &mutex_ptr->mutex);
#ifdef DEBUG_TRUSTM_MUTEX
    for (int i = 0; i < sizeof(pthread_mutex_t); i++) {
        printf("%x", ((char *)&mutex_ptr->mutex)[i]);
    }
    printf("\n");
#endif
    mutex.ptr = &mutex_ptr->mutex;
    mutex.pid = &mutex_ptr->pid;
    mutex.name = (char *)malloc(NAME_MAX + 1);
    strcpy(mutex.name, name);
    TRUSTM_MUTEX_DBGFN2("<");
    return mutex;
}

static int shared_mutex_close(shared_mutex_t mutex) {
    if (munmap((void *)mutex.ptr, sizeof(pthread_mutex_t))) {
        perror("munmap");
        return -1;
    }
    mutex.ptr = NULL;
    if (close(mutex.shm_fd)) {
        perror("close");
        return -1;
    }
    mutex.shm_fd = 0;
    free(mutex.name);
    return 0;
}

static int shared_mutex_destroy(shared_mutex_t mutex) {
    if ((errno = pthread_mutex_destroy(mutex.ptr))) {
        perror("pthread_mutex_destroy");
        return -1;
    }
    if (munmap((void *)mutex.ptr, sizeof(pthread_mutex_t))) {
        perror("munmap");
        return -1;
    }
    mutex.ptr = NULL;
    if (close(mutex.shm_fd)) {
        perror("close");
        return -1;
    }
    mutex.shm_fd = 0;
    if (shm_unlink(mutex.name)) {
        perror("shm_unlink");
        return -1;
    }
    free(mutex.name);
    return 0;
}

pal_status_t pal_shm_mutex_acquire(shared_mutex_t *shm_mutex, const char *mutex_name) {
    int result;
    TRUSTM_MUTEX_DBGFN1(">");

    *shm_mutex = shared_mutex_init(mutex_name);
    if (shm_mutex->ptr == NULL) {
        TRUSTM_MUTEX_DBGFN1("Mutex create failed\n");
        return PAL_STATUS_FAILURE;
    }

    result = pthread_mutex_lock(shm_mutex->ptr);
    TRUSTM_MUTEX_DBGFN1("Lock Mutex:%s: %x\n", mutex_name, (unsigned int)shm_mutex->ptr);
    if (result == EOWNERDEAD) {
        TRUSTM_MUTEX_DBGFN1("process owner dead, make it consistent\n");
        result = pthread_mutex_consistent(shm_mutex->ptr);
        if (result != 0) {
            perror("pthread_mutex_consistent error");
            return PAL_STATUS_FAILURE;
        }
    }
    if (shm_mutex->created) {
        TRUSTM_MUTEX_DBGFN1("The mutex was just created %x\n", *shm_mutex->pid);
        *shm_mutex->pid = EMPTY_PID;
    }
    TRUSTM_MUTEX_DBGFN1("<");
    return PAL_STATUS_SUCCESS;
}

/**********************************************************************
 * pal_shm_mutex_release(shared_mutex_t *shm_mutex)
 **********************************************************************/
void pal_shm_mutex_release(shared_mutex_t *shm_mutex) {
    TRUSTM_MUTEX_DBGFN1(">");
    pthread_mutex_unlock(shm_mutex->ptr);
    TRUSTM_MUTEX_DBGFN1("mutex unlock:%s:%x", shm_mutex->name, (unsigned int)shm_mutex->ptr);

    shared_mutex_close(*shm_mutex);
    TRUSTM_MUTEX_DBGFN1("mutex closed");

    TRUSTM_MUTEX_DBGFN1("<");
}
