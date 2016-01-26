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
#include "TMath.h"

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

    // check data by looping over all data points
    Bool_t badData = kFALSE;
    for (Int_t i = 0; i < nEntries; i++)
    {
        // get entry
        const RooArgSet* set = fData->get(i);

        // loop over variables
        Bool_t badRow = kFALSE;
        for (Int_t j = 0; j < fNVar; j++)
        {
            RooRealVar* var = (RooRealVar*)set->find(fVar[j]->GetName());

            // check value
            if (TMath::IsNaN(var->getVal()))
            {
                badRow = kTRUE;
                break;
            }
        }

        // check row
        if (badRow)
        {
            // format bad row
            TString tmp;
            for (Int_t j = 0; j < fNVar; j++)
            {
                RooRealVar* var = (RooRealVar*)set->find(fVar[j]->GetName());
                tmp.Append(TString::Format("%s: %e  ", fVar[j]->GetName(), var->getVal()));
            }
            for (Int_t j = 0; j < fNVarAux; j++)
            {
                RooRealVar* var = (RooRealVar*)set->find(fVarAux[j]->GetName());
                tmp.Append(TString::Format("%s: %e  ", fVarAux[j]->GetName(), var->getVal()));
            }

            Error("LoadData", "NaN at row %d: %s", i, tmp.Data());
            badData = kTRUE;
        }
    }

    // check overall data status
    if (badData)
    {
        Error("LoadData", "Invalid data in data chain!");
        return kFALSE;
    }
    else
        return kTRUE;
}

