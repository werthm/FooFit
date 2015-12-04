/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFFooFit                                                             //
//                                                                      //
// Namespace for global methods and variables.                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifdef _WIN32
#include <windows.h>
#elif MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

#include "FFFooFit.h"

namespace FFFooFit
{
    Int_t gUseNCPU = 1;
    Int_t gParStrat = 0;
}

//______________________________________________________________________________
Int_t FFFooFit::GetNumberOfCPUs()
{
    // Return the number of CPU cores on a system.
    // Supported on Linux, Mac OS X and Windows.
    // Source: http://www.cprogramming.com/snippets/source-code/find-the-number-
    //         of-cpu-cores-for-windows-mac-or-linux

    #ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
    #elif MACOS
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if(count < 1) {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if(count < 1) { count = 1; }
    }
    return count;
    #else
    return sysconf(_SC_NPROCESSORS_ONLN);
    #endif
}

