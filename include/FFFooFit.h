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

namespace FFFooFit
{
    extern Int_t gUseNCPU;      // number of CPUs to use
    extern Int_t gParStrat;     // parallelization strategy

    Int_t GetNumberOfCPUs();
}

#endif

