#include <sys/lib/input/Keyboard.h>
#include <sys/lib/Library.h>

namespace pmlib
{
    bool KeyboardDown(Key key) { return ((MTYPE_KB_DOWN)MTABLE_ENTRIES[MTYPEID_KB_DOWN].addr)((uint8_t)key); }

    bool KeyboardUp(Key key) { return ((MTYPE_KB_UP)MTABLE_ENTRIES[MTYPEID_KB_UP].addr)((uint8_t)key); }
}