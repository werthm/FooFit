/*************************************************************************
 * Author: Dominik Werthmueller, 2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelLandau                                                     //
//                                                                      //
// Landau model for RooFit.                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooLandau.h"

#include "FFRooModelLandau.h"
#include "RooGaussModel.h"
#include "RooFFTConvPdf.h"

ClassImp(FFRooModelLandau)

//______________________________________________________________________________
FFRooModelLandau::FFRooModelLandau(const Char_t* name, const Char_t* title,
                                   Bool_t gaussConvol)
    : FFRooModel(name, title, gaussConvol ? 4 : 2)
{
    // Constructor.

    // init members
    fIsConvol = gaussConvol;

    // add the mean parameter
    TString tmp = TString::Format("%s_Mean", GetName());
    AddParameter(0, tmp.Data(), tmp.Data());

    // add the sigma parameter
    tmp = TString::Format("%s_Sigma", GetName());
    AddParameter(1, tmp.Data(), tmp.Data());

    // add convolution parameters
    if (fIsConvol)
    {
        tmp = TString::Format("%s_Conv_GMean", GetName());
        AddParameter(2, tmp.Data(), tmp.Data());
        tmp = TString::Format("%s_Conv_GSigma", GetName());
        AddParameter(3, tmp.Data(), tmp.Data());
    }
}

//______________________________________________________________________________
void FFRooModelLandau::BuildModel(RooAbsReal** vars)
{
    // Build the model using the variables 'vars'.

    // create the model pdf
    if (fPdf)
        delete fPdf;
    if (fIsConvol)
    {
        // delete old pdfs
        if (fPdfIntr)
            delete fPdfIntr;
        if (fPdfConv)
            delete fPdfConv;

        // create pdfs
        TString tmp;
        tmp = TString::Format("%s_Conv_Intr", GetName());
        fPdfIntr = new RooLandau(tmp.Data(), tmp.Data(), *vars[0], *fPar[0], *fPar[1]);
        tmp = TString::Format("%s_Conv_Gauss", GetName());
        fPdfConv = new RooGaussModel(tmp.Data(), tmp.Data(), *((RooRealVar*)vars[0]), *fPar[2], *fPar[3]);
        ((RooRealVar*)vars[0])->setBins(10000, "cache");
        fPdf = new RooFFTConvPdf(GetName(), GetTitle(), *((RooRealVar*)vars[0]), *fPdfIntr, *fPdfConv);
    }
    else
    {
        // create pdf
        fPdf = new RooLandau(GetName(), GetTitle(), *vars[0], *fPar[0], *fPar[1]);
    }
}

