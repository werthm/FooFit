/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelPol                                                        //
//                                                                      //
// 1-dim. polynomial model for RooFit.                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooPolynomial.h"
#include "RooRealVar.h"

#include "FFRooModelPol.h"

ClassImp(FFRooModelPol)

//______________________________________________________________________________
FFRooModelPol::FFRooModelPol(const Char_t* name, const Char_t* title, Int_t nOrder)
    : FFRooModel(name, title, nOrder+1)
{
    // Constructor.

    Char_t tmp[256];

    // add the coefficient parameters
    for (Int_t i = 0; i < fNPar; i++)
    {
        sprintf(tmp, "%s_PolCoeff_%d", GetName(), i);
        AddParameter(i, tmp, tmp);
    }
}

//______________________________________________________________________________
void FFRooModelPol::BuildModel(RooRealVar** vars)
{
    // Build the model using the variables 'vars'.

    // prepare list of coefficients
    RooArgList coeffList;
    for (Int_t i = 0; i < fNPar; i++) coeffList.add(*fPar[i]);

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooPolynomial(GetName(), GetTitle(), *vars[0], coeffList, 0);
}

