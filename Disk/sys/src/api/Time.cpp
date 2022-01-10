#include <sys/lib/api/Time.h>
#include <sys/lib/api/Library.h>

namespace pmlib
{
    DateTime GetCurrentTime() { return ((MTYPE_GET_TIME)MTABLE_ENTRIES[MTYPEID_GET_TIME].addr)(); }

    char* GetTimeString(DateTime time, bool military, bool second, char* buffer)
    {
        char time_temp[64];
        memset(time_temp, 0, 64);

        if (military) { itoa(time.Hour, time_temp, 10); }
        else 
        {
            if (time.Hour > 12) { itoa(time.Hour - 12, time_temp, 10); }
            else { itoa(time.Hour, time_temp, 10); }
        }
        if (time.Hour < 10) { strcat(buffer, "0"); }
        strcat(buffer, time_temp);
        strcat(buffer, ":");

        
        itoa(time.Minute, time_temp, 10);
        if (time.Minute < 10) { strcat(buffer, "0"); }
        strcat(buffer, time_temp);

        if (second)
        {
            strcat(buffer, ":");
            itoa(time.Second, time_temp, 10);
            if (time.Second < 10) { strcat(buffer, "0"); }
            strcat(buffer, time_temp);
        }

        if (!military)
        {
            if (time.Hour > 12) { strcat(buffer, " PM"); } else { strcat(buffer, " AM"); }
        }

        return buffer;
    }
}