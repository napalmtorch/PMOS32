#include <sys/lib/input/Mouse.h>
#include <sys/lib/Library.h>

namespace pmlib
{
    uint32_t GetMouseX() { return ((MTYPE_MS_GETX)MTABLE_ENTRIES[MTYPEID_MS_GETX].addr)(); }

    uint32_t GetMouseY() { return ((MTYPE_MS_GETY)MTABLE_ENTRIES[MTYPEID_MS_GETY].addr)(); }

    bool GetMouseLeftBtn() { return ((MTYPE_MS_GETLEFT)MTABLE_ENTRIES[MTYPEID_MS_GETLEFT].addr)(); }

    bool GetMouseRightBtn()  { return ((MTYPE_MS_GETRIGHT)MTABLE_ENTRIES[MTYPEID_MS_GETRIGHT].addr)(); }
}