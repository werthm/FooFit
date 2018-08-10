/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelPol                                                        //
//                                                                      //
// 1-dim. polynomial model for RooFit.                                  //
// Note: Parameters start at order 0.                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooPolynomial.h"

#include "FFRooModelPol.h"

ClassImp(FFRooModelPol)

//______________________________________________________________________________
FFRooModelPol::FFRooModelPol(const Char_t* name, const Char_t* title, Int_t nOrder)
    : FFRooModel(name, title, nOrder+1)
{
    // Constructor.

    // add the coefficient parameters
    for (Int_t i = 0; i < fNPar; i++)
    {
        TString tmp = TString::Format("%s_PolCoeff_%d", GetName(), i);
        AddParameter(i, tmp.Data(), tmp.Data());
    }
}

//______________________________________________________________________________
void FFRooModelPol::BuildModel(RooAbsReal** vars)
{
    // Build the model using the variables 'vars'.

    // prepare list of coefficients
    RooArgList coeffList;
    for (Int_t i = 0; i < fNPar; i++) coeffList.add(*fPar[i]);

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooPolynomial(GetName(), GetTitle(), *vars[0], coeffList, 0);
}

