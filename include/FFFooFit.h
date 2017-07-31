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


#ifndef FOOFIT_FFFooFit
#define FOOFIT_FFFooFit

#include "Rtypes.h"

class TChain;

namespace FFFooFit
{
    extern Int_t gUseNCPU;      // number of CPUs to use
    extern Int_t gParStrat;     // parallelization strategy

    Int_t GetNumberOfCPUs();
    Bool_t LoadFilesToChain(const Char_t* loc, TChain* chain);
    Bool_t FileExists(const Char_t* f);
}

#endif

