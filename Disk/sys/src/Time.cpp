#include <sys/lib/Time.h>
#include <sys/lib/Library.h>

namespace pmlib
{
    DateTime GetCurrentTime() { return ((MTYPE_GET_TIME)MTABLE_ENTRIES[MTYPEID_GET_TIME].addr)(); }
}