/*************************************************************************
 * Author: Dominik Werthmueller, 2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelGaussBifur                                                 //
//                                                                      //
// 1-dim. bifurcated Gaussian model for RooFit.                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooBifurGauss.h"

#include "FFRooModelGaussBifur.h"

ClassImp(FFRooModelGaussBifur)

//______________________________________________________________________________
FFRooModelGaussBifur::FFRooModelGaussBifur(const Char_t* name, const Char_t* title)
    : FFRooModel(name, title, 3)
{
    // Constructor.

    // add the mean parameter
    TString tmp = TString::Format("%s_Mean", GetName());
    AddParameter(0, tmp.Data(), tmp.Data());

    // add the sigmaL parameter
    tmp = TString::Format("%s_SigmaL", GetName());
    AddParameter(1, tmp.Data(), tmp.Data());

    // add the sigmaR parameter
    tmp = TString::Format("%s_SigmaR", GetName());
    AddParameter(2, tmp.Data(), tmp.Data());
}

//______________________________________________________________________________
void FFRooModelGaussBifur::BuildModel(RooAbsReal** vars)
{
    // Build the model using the variables 'vars'.

    // create the model pdf
    if (fPdf)
        delete fPdf;
    fPdf = new RooBifurGauss(GetName(), GetTitle(), *vars[0], *fPar[0], *fPar[1], *fPar[2]);
}

