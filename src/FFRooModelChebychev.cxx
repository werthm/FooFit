/*************************************************************************
 * Author: Dominik Werthmueller, 2018
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelChebychev                                                  //
//                                                                      //
// 1-dim. Chebychev polynomial model for RooFit.                        //
// Note: Parameters start at order 1.                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooChebychev.h"

#include "FFRooModelChebychev.h"

ClassImp(FFRooModelChebychev)

//______________________________________________________________________________
FFRooModelChebychev::FFRooModelChebychev(const Char_t* name, const Char_t* title, Int_t nOrder)
    : FFRooModel(name, title, nOrder)
{
    // Constructor.

    // add the coefficient parameters
    for (Int_t i = 0; i < fNPar; i++)
    {
        TString tmp = TString::Format("%s_ChebPolCoeff_%d", GetName(), i);
        AddParameter(i, tmp.Data(), tmp.Data());
    }
}

//______________________________________________________________________________
void FFRooModelChebychev::BuildModel(RooAbsReal** vars)
{
    // Build the model using the variables 'vars'.

    // prepare list of coefficients
    RooArgList coeffList;
    for (Int_t i = 0; i < fNPar; i++) coeffList.add(*fPar[i]);

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooChebychev(GetName(), GetTitle(), *vars[0], coeffList);
}

