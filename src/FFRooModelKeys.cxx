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


#include "TTree.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooNDKeysPdf.h"

#include "FFRooModelKeys.h"

ClassImp(FFRooModelKeys)

//______________________________________________________________________________
FFRooModelKeys::FFRooModelKeys(const Char_t* name, const Char_t* title, Int_t nDim, TTree* tree,
                               const Char_t* opt, Double_t rho, Int_t nSigma, Bool_t rotate)
    : FFRooModel(name, title, 0)
{
    // Constructor.
    // See RooNDKeysPdf for meaning of parameters.

    // init members
    fNDim = nDim;
    fTree = tree;
    fDataSet = 0;
    fOpt = opt;
    fRho = rho;
    fNSigma = nSigma;
    fRotate = rotate;
}

//______________________________________________________________________________
FFRooModelKeys::~FFRooModelKeys()
{
    // Destructor.

    if (fTree) delete fTree;
    if (fDataSet) delete fDataSet;
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
    fDataSet = new RooDataSet(fTree->GetName(), fTree->GetTitle(), varSet, RooFit::Import(*fTree));

    // user info
    Int_t nEntries = fDataSet->numEntries();
    Info("BuildModel", "Entries in data tree      : %.9e", (Double_t)fTree->GetEntries());
    Info("BuildModel", "Entries in RooFit dataset : %.9e", (Double_t)nEntries);

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooNDKeysPdf(GetName(), GetTitle(), varList, *fDataSet, fOpt.Data(), fRho, fNSigma, fRotate);
}

