#include <stdlib.h>

#include "FreeRTOS.h"
#include "timers.h"

#include "optiga/pal/pal_os_memory.h"
#include "optiga/pal/pal.h"

void * pal_os_malloc(uint32_t block_size)
{
  return pvPortMalloc(block_size);
}

void * pal_os_calloc(uint32_t number_of_blocks , uint32_t block_size)
{
  void *p = pvPortMalloc(number_of_blocks * block_size);
  if (p != NULL)
  {
    memset(p, '\0', number_of_blocks * block_size);
  }

  return p;
}

void pal_os_free(void *p)
{
  vPortFree(p);
}

void pal_os_memcpy(void * p_destination, const void * p_source, uint32_t size)
{
    memcpy(p_destination, p_source, size);
}

void pal_os_memset(void * p_buffer, uint32_t value, uint32_t size)
{
    memset(p_buffer, (int32_t)value, size);
}
