/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelExpo                                                       //
//                                                                      //
// 1-dim. exponential model for RooFit.                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooExponential.h"

#include "FFRooModelExpo.h"

ClassImp(FFRooModelExpo)

//______________________________________________________________________________
FFRooModelExpo::FFRooModelExpo(const Char_t* name, const Char_t* title)
    : FFRooModel(name, title, 1)
{
    // Constructor.

    Char_t tmp[256];

    // add the slope parameter
    sprintf(tmp, "%s_Slope", GetName());
    AddParameter(0, tmp, tmp);
}

//______________________________________________________________________________
void FFRooModelExpo::BuildModel(RooAbsReal** vars)
{
    // Build the model using the variables 'vars'.

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooExponential(GetName(), GetTitle(), *vars[0], *fPar[0]);
}

