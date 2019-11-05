/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFFooFit                                                             //
//                                                                      //
// Namespace for global methods and variables.                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFFooFit
#define FOOFIT_FFFooFit

#include "Rtypes.h"

# define FOOFIT_VERSION "0.1.0"

class TChain;

namespace FFFooFit
{
    extern Int_t gUseNCPU;      // number of CPUs to use
    extern Int_t gParStrat;     // parallelization strategy

    Int_t GetNumberOfCPUs();
    Bool_t LoadFilesToChain(const Char_t* loc, TChain* chain,
                            const Char_t* wildCard = 0);
    Bool_t FileExists(const Char_t* f);

    Int_t IndexOf(const Char_t* s1, const Char_t* s2, UInt_t p = 0);
    Int_t LastIndexOf(const Char_t* s, Char_t c);
    TString ExpandPath(const Char_t* s);
    TString ExtractFileName(const Char_t* s);
    TString ExtractDirectory(const Char_t* s);
}

#endif

