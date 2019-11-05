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
Bool_t FFFooFit::LoadFilesToChain(const Char_t* loc, TChain* chain,
                                  const Char_t* wildCard)
{
    // Add all ROOT files in the location 'loc' to the TChain 'chain'.
    // If 'loc' is a single ROOT file, it will be added to the first chain.
    // If 'wildCard' is non-zero, look for files in 'loc' using this file wild card.
    // Return kTRUE if the loading was successful without any error, otherwise kFALSE.

    // read location
    TString loc_ex = ExpandPath(loc);

    // check if file exists
    if (!FileExists(loc_ex.Data()) && wildCard == 0)
    {
        // try to use parent directory and file name as wild card
        TString file = ExtractFileName(loc_ex.Data());
        TString parent = ExtractDirectory(loc_ex.Data());
        if (!LoadFilesToChain(parent.Data(), chain, file.Data()))
        {
            Error("FFFooFit::LoadFilesToChain", "File or directory '%s' does not exist!", loc_ex.Data());
            return kFALSE;
        }
        else
        {
            return kTRUE;
        }
    }

    // check if location is a single file
    TSystemFile file(loc_ex.Data(), "rawfile");
    if (!file.IsDirectory())
    {
        // check if it is a ROOT file
        TString str(loc_ex.Data());
        if (str.EndsWith(".root"))
        {
            Info("FFFooFit::LoadFilesToChain", "[1] Adding '%s' to chain 0", loc_ex.Data());
            chain->Add(loc_ex.Data());
            return kTRUE;
        }
        else
        {
            Error("FFFooFit::LoadFilesToChain", "'%s' does not seem to be a ROOT file!", loc_ex.Data());
            return kFALSE;
        }
    }

    // try to get directory content
    TSystemDirectory dir("rawdir", loc_ex.Data());
    TList* list = dir.GetListOfFiles();
    if (!list)
    {
        Error("FFFooFit::LoadFilesToChain", "'%s' is not a directory!", loc_ex.Data());
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
            TString tmp = TString::Format("%s/%s", loc_ex.Data(), f->GetName());

            // check wild card
            if (wildCard && !str.Contains(wildCard))
                continue;

            // user information
            Info("FFFooFit::LoadFilesToChain", "[%d] Adding '%s' to chain", n+1, tmp.Data());

            // add file to chains
            chain->Add(tmp.Data());

            // count file
            n++;
        }
    }

    // clean-up
    delete list;

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFFooFit::FileExists(const Char_t* f)
{
    // Return kTRUE if the file 'f' exists, otherwise return kFALSE.

    // expand filename
    Char_t* fnt = gSystem->ExpandPathName(f);
    TString fn = fnt;
    delete fnt;

    // check file
    if (!gSystem->AccessPathName(fn.Data())) return kTRUE;
    else return kFALSE;
}

//______________________________________________________________________________
Int_t FFFooFit::IndexOf(const Char_t* s1, const Char_t* s2, UInt_t p)
{
    // Returns the position of the first occurrence of the string s2
    // in the string s1 after position p. Returns -1 if s2 was not found.

    const Char_t* pos = strstr(s1+p, s2);
    if (pos) return pos-s1;
    else return -1;
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

