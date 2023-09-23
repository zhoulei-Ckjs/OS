#include "linux/mm.h"

void virtual_memory_init()
{
    for (int i = 0; i < 3; ++i)
    {
        void* p = get_free_page();
        free_page(p);
    }
}