/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelGauss                                                      //
//                                                                      //
// 1-dim. Gaussian model for RooFit.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooGaussian.h"

#include "FFRooModelGauss.h"

ClassImp(FFRooModelGauss)

//______________________________________________________________________________
FFRooModelGauss::FFRooModelGauss(const Char_t* name, const Char_t* title)
    : FFRooModel(name, title, 2)
{
    // Constructor.

    Char_t tmp[256];

    // add the mean parameter
    sprintf(tmp, "%s_Mean", GetName());
    AddParameter(0, tmp, tmp);

    // add the sigma parameter
    sprintf(tmp, "%s_Sigma", GetName());
    AddParameter(1, tmp, tmp);
}

//______________________________________________________________________________
void FFRooModelGauss::BuildModel(RooAbsReal** vars)
{
    // Build the model using the variables 'vars'.

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooGaussian(GetName(), GetTitle(), *vars[0], *fPar[0], *fPar[1]);
}

