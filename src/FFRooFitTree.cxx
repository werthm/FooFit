/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2018
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
#include "RooDataHist.h"
#include "RooGlobalFunc.h"
#include "TTree.h"
#include "TMath.h"

#include "FFRooFitTree.h"

ClassImp(FFRooFitTree)

//______________________________________________________________________________
FFRooFitTree::FFRooFitTree(TTree* tree, Int_t nVar,
                           const Char_t* name, const Char_t* title,
                           const Char_t* weightVar, Bool_t binnedFit)
    : FFRooFit(nVar, name, title)
{
    // Constructor using the tree 'tree' and 'nVar' fit variables.
    // If 'weightVar' is non-zero, create a weighted dataset using this
    // tree variable to read the weights from.
    // If 'binnedFit' is kTRUE, perform a binned fit of the data set.

    // init members
    fTree = tree;
    if (weightVar)
    {
        fWeights = new RooRealVar(weightVar, "Event Weights",
                                  -RooNumber::infinity(), RooNumber::infinity());
        AddAuxVariable(fWeights);
        Info("FFRooFitTree", "Using variable '%s' as weights", weightVar);
    }
    else
        fWeights = 0;
    fIsBinnedFit = binnedFit;
}

//______________________________________________________________________________
FFRooFitTree::~FFRooFitTree()
{
    // Destructor.

    if (fWeights) delete fWeights;
}

//______________________________________________________________________________
Bool_t FFRooFitTree::LoadData()
{
    // Load the fit data.
    // Return kTRUE on success, otherwise kFALSE.

    // check fit variables
    if (!CheckVariables()) return kFALSE;

    // check tree
    if (!fTree)
    {
        Error("LoadData", "Input data tree was not set!");
        return kFALSE;
    }

    // create argument set of variables and auxiliary variables
    RooArgSet varSet;
    for (Int_t i = 0; i < fNVar; i++) varSet.add(*fVar[i]);
    for (Int_t i = 0; i < fNVarAux; i++) varSet.add(*fVarAux[i]);

    // create RooFit dataset
    if (fData) delete fData;
    if (fWeights)
    {
        fData = new RooDataSet(fTree->GetName(), fTree->GetTitle(), varSet,
                               RooFit::Import(*fTree), RooFit::WeightVar(*fWeights));
    }
    else
    {
        fData = new RooDataSet(fTree->GetName(), fTree->GetTitle(), varSet,
                               RooFit::Import(*fTree));
    }

    // user info
    Int_t nEntries = fData->numEntries();
    Info("LoadData", "Entries in data tree      : %.9e", (Double_t)fTree->GetEntries());
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

        // check weight
        if (fData->isWeighted())
        {
            Double_t w = fData->weight();

            if (w == 0)
                Warning("LoadData", "Event weight at row %d is zero", i);
            if (TMath::IsNaN(w))
            {
                Error("LoadData", "Event weight at row %d is NaN!", i);
                badData = kTRUE;
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
                if (var)
                    tmp.Append(TString::Format("%s: %e  ", fVar[j]->GetName(), var->getVal()));
            }
            for (Int_t j = 0; j < fNVarAux; j++)
            {
                RooRealVar* var = (RooRealVar*)set->find(fVarAux[j]->GetName());
                if (var)
                    tmp.Append(TString::Format("%s: %e  ", fVarAux[j]->GetName(), var->getVal()));
            }

            Error("LoadData", "NaN at row %d: %s", i, tmp.Data());
            badData = kTRUE;
        }
    }

    // check overall data status
    if (badData)
    {
        Error("LoadData", "Invalid data in data tree!");
        return kFALSE;
    }

    // convert to binned data set if requested
    if (fIsBinnedFit)
    {
        RooAbsData* old_data = fData;
        fData = new RooDataHist(fTree->GetName(), fTree->GetTitle(), varSet, *old_data);
        delete old_data;
    }

    return kTRUE;
}

