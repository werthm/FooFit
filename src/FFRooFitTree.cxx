/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitTree                                                         //
//                                                                      //
// Class performing RooFit fits of trees.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGlobalFunc.h"
#include "TChain.h"

#include "FFRooFitTree.h"

ClassImp(FFRooFitTree)

//______________________________________________________________________________
FFRooFitTree::FFRooFitTree(TChain* chain, Int_t nVar,
                           const Char_t* name, const Char_t* title)
    : FFRooFit(nVar, name, title)
{
    // Constructor using the tree chain 'chain' and 'nVar' fit variables.

    // init members
    fChain = chain;
}

//______________________________________________________________________________
Bool_t FFRooFitTree::LoadData()
{
    // Load the fit data.
    // Return kTRUE on success, otherwise kFALSE.

    // check fit variables
    if (!CheckVariables()) return kFALSE;

    // check chain
    if (!fChain)
    {
        Error("LoadData", "Input data chain was not set!");
        return kFALSE;
    }

    // create argument set of variables and auxiliary variables
    RooArgSet varSet;
    for (Int_t i = 0; i < fNVar; i++) varSet.add(*fVar[i]);
    for (Int_t i = 0; i < fNVarAux; i++) varSet.add(*fVarAux[i]);

    // create RooFit dataset
    if (fData) delete fData;
    fData = new RooDataSet(fChain->GetName(), fChain->GetTitle(), varSet, RooFit::Import(*fChain));

    // user info
    Int_t nEntries = fData->numEntries();
    Info("LoadData", "Entries in data chain     : %.9e", (Double_t)fChain->GetEntries());
    Info("LoadData", "Entries in RooFit dataset : %.9e", (Double_t)nEntries);

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFitTree::PreFit()
{
    // Perform tasks before fitting.
    // Return kTRUE on success, otherwise kFALSE.

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFitTree::PostFit()
{
    // Perform tasks after fitting.
    // Return kTRUE on success, otherwise kFALSE.

    return kTRUE;
}

