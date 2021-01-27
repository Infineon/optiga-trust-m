#include <stdlib.h>

#include "FreeRTOS.h"
#include "timers.h"

#include "optiga/pal/pal_os_memory.h"
#include "optiga/pal/pal.h"

void *pal_os_malloc(size_t size)
{
  return pvPortMalloc(size);
}

void *pal_os_calloc(size_t num, size_t size)
{
  void *p = pvPortMalloc(num * size);
  if (p != NULL)
  {
    memset(p, '\0', num * size);
  }

  return p;
}

void pal_os_free(void *p)
{
  vPortFree(p);
}
