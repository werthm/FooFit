/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2017
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

#include "TChain.h"
#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TSystem.h"
#include "TError.h"

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

//______________________________________________________________________________
Bool_t FFFooFit::LoadFilesToChain(const Char_t* loc, TChain* chain)
{
    // Add all ROOT files in the location 'loc' to the TChain 'chain'.
    // Return kTRUE if the loading was successful without any error, otherwise kFALSE.

    Char_t tmp[256];

    // read location
    Char_t* loc_ex = gSystem->ExpandPathName(loc);

    // check if file exits
    if (!FileExists(loc_ex))
    {
        Error("FFFooFit::LoadFilesToChain", "File or directory '%s' does not exist!", loc_ex);
        delete loc_ex;
        return kFALSE;
    }

    // check if location is a single file
    TSystemFile file(loc_ex, "rawfile");
    if (!file.IsDirectory())
    {
        // check if it is a ROOT file
        TString str(loc_ex);
        if (str.EndsWith(".root"))
        {
            Info("FFFooFit::LoadFilesToChain", "[1] Adding '%s' to chain 0", loc_ex);
            chain->Add(loc_ex);
            delete loc_ex;
            return kTRUE;
        }
        else
        {
            Error("FFFooFit::LoadFilesToChain", "'%s' does not seem to be a ROOT file!", loc_ex);
            delete loc_ex;
            return kFALSE;
        }
    }

    // try to get directory content
    TSystemDirectory dir("rawdir", loc_ex);
    TList* list = dir.GetListOfFiles();
    if (!list)
    {
        Error("FFFooFit::LoadFilesToChain", "'%s' is not a directory!", loc_ex);
        delete loc_ex;
        return kFALSE;
    }

    // sort files
    list->Sort();

    // loop over directory content
    Int_t n = 0;
    TIter next(list);
    TSystemFile* f;
    while ((f = (TSystemFile*)next()))
    {
        // look for ROOT files
        TString str(f->GetName());
        if (str.EndsWith(".root"))
        {
            // full path
            sprintf(tmp, "%s/%s", loc_ex, f->GetName());

            // user information
            Info("FFFooFit::LoadFilesToChain", "[%d] Adding '%s' to chain", n+1, tmp);

            // add file to chains
            chain->Add(tmp);

            // count file
            n++;
        }
    }

    // clean-up
    delete list;
    delete loc_ex;

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFFooFit::FileExists(const Char_t* f)
{
    // Return kTRUE if the file 'f' exists, otherwise return kFALSE.

    Char_t fn[256];

    // expand filename
    Char_t* fnt = gSystem->ExpandPathName(f);
    strcpy(fn, fnt);
    delete fnt;

    // check file
    if (!gSystem->AccessPathName(fn)) return kTRUE;
    else return kFALSE;
}

//______________________________________________________________________________
Int_t FFFooFit::LastIndexOf(const Char_t* s, Char_t c)
{
    // Returns the position of the last occurrence of the character c
    // in the string s. Returns -1 if c was not found.

    const Char_t* pos = strrchr(s, (Int_t)c);
    if (pos)
        return pos-s;
    else
        return -1;
}

//______________________________________________________________________________
TString FFFooFit::ExpandPath(const Char_t* s)
{
    // Expand all shell variables in the string 's' and return the result.

    return TString(gSystem->ExpandPathName(s));
}

//______________________________________________________________________________
TString FFFooFit::ExtractFileName(const Char_t* s)
{
    // Extracts the file name of a file given by its full Unix paths in
    // the string s.

    // create TString
    TString out(s);

    // search last slash
    Int_t pos = LastIndexOf(s, '/');

    // remove leading path
    if (pos != -1)
        out.Remove(0, pos+1);

    return out;
}

//______________________________________________________________________________
TString FFFooFit::ExtractDirectory(const Char_t* s)
{
    // Extracts the parent directory out of the full Unix path 's'.

    // create TString
    TString out(s);

    // search last slash
    Int_t pos = LastIndexOf(s, '/');

    // format directory
    if (pos == -1)
        out = ".";
    else
    {
        out.Remove(pos, strlen(s)-pos);
        if (!out.Length())
            out = ".";
    }

    return out;
}

