/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelKeys                                                       //
//                                                                      //
// Class representing a model for RooFit using the n-dimensional kernel //
// estimation pdf of RooNDKeysPdf.                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TChain.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooNDKeysPdf.h"

#include "FFRooModelKeys.h"

ClassImp(FFRooModelKeys)

//______________________________________________________________________________
FFRooModelKeys::FFRooModelKeys(const Char_t* name, const Char_t* title, Int_t nDim, TChain* chain,
                               const Char_t* opt, Double_t rho, Int_t nSigma, Bool_t rotate)
    : FFRooModel(name, title, 0)
{
    // Constructor.
    // See RooNDKeysPdf for meaning of parameters.

    // init members
    fNDim = nDim;
    fChain = chain;
    fDataSet = 0;
    fOpt = new Char_t[256];
    strcpy(fOpt, opt);
    fRho = rho;
    fNSigma = nSigma;
    fRotate = rotate;
}

//______________________________________________________________________________
FFRooModelKeys::~FFRooModelKeys()
{
    // Destructor.

    if (fDataSet) delete fDataSet;
    if (fOpt) delete [] fOpt;
}

//______________________________________________________________________________
void FFRooModelKeys::BuildModel(RooRealVar** vars)
{
    // Build the model using the variables 'vars'.

    // create argument set and list of variables
    RooArgSet varSet;
    RooArgList varList;
    for (Int_t i = 0; i < fNDim; i++)
    {
        varSet.add(*vars[i]);
        varList.add(*vars[i]);
    }

    // create RooFit dataset
    if (fDataSet) delete fDataSet;
    fDataSet = new RooDataSet(fChain->GetName(), fChain->GetTitle(), varSet, RooFit::Import(*fChain));

    // user info
    Int_t nEntries = fDataSet->numEntries();
    Info("BuildModel", "Entries in data chain     : %.9e", (Double_t)fChain->GetEntries());
    Info("BuildModel", "Entries in RooFit dataset : %.9e", (Double_t)nEntries);

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooNDKeysPdf(GetName(), GetTitle(), varList, *fDataSet, fOpt, fRho, fNSigma, fRotate);
}

