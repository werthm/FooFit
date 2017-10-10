/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelHist                                                       //
//                                                                      //
// Class representing a model from a histogram for RooFit.              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"

#include "FFRooModelHist.h"

ClassImp(FFRooModelHist)

//______________________________________________________________________________
FFRooModelHist::FFRooModelHist(const Char_t* name, const Char_t* title, TH1* hist,
                               Bool_t addShiftPar)
    : FFRooModel(name, title, addShiftPar ? hist->GetDimension() : 0)
{
    // Constructor.

    Char_t tmp[256];

    // init members
    fNDim = hist->GetDimension();
    fHist = hist;
    fTree = 0;
    fWeightVar = 0;
    fDataHist = 0;

    // add shift parameters
    if (addShiftPar)
    {
        // loop over dimensions
        for (Int_t i = 0; i < fNDim; i++)
        {
            // add shift parameter
            sprintf(tmp, "%s_Shift", GetName());
            AddParameter(i, tmp, tmp);
        }
    }
}

//______________________________________________________________________________
FFRooModelHist::FFRooModelHist(const Char_t* name, const Char_t* title, Int_t nDim, TTree* tree,
                               const Char_t* weightVar, Bool_t addShiftPar)
    : FFRooModel(name, title, addShiftPar ? nDim : 0)
{
    // Constructor.

    Char_t tmp[256];

    // init members
    fNDim = nDim;
    fHist = 0;
    fTree = tree;
    if (weightVar)
    {
        fWeightVar = new Char_t[256];
        strcpy(fWeightVar, weightVar);
    }
    else
    {
        fWeightVar = 0;
    }
    fDataHist = 0;

    // add shift parameters
    if (addShiftPar)
    {
        // loop over dimensions
        for (Int_t i = 0; i < fNDim; i++)
        {
            // add shift parameter
            sprintf(tmp, "%s_Shift", GetName());
            AddParameter(i, tmp, tmp);
        }
    }
}

//______________________________________________________________________________
FFRooModelHist::~FFRooModelHist()
{
    // Destructor.

    if (fHist) delete fHist;
    if (fTree) delete fTree;
    if (fWeightVar) delete [] fWeightVar;
    if (fDataHist) delete fDataHist;
}

//______________________________________________________________________________
void FFRooModelHist::BuildModel(RooRealVar** vars)
{
    // Build the model using the variables 'vars'.

    Char_t tmp[256];

    // prepare variable set
    RooArgSet varSet;
    for (Int_t i = 0; i < fNDim; i++)
        varSet.add(*vars[i]);

    // create binned input data
    if (!fHist && fTree)
    {
        // check dimension
        if (fNDim == 1)
        {
            // create the histogram
            fHist = new TH1F(TString::Format("hist_%s_%s",
                                             vars[0]->GetName(),
                                             GetName()).Data(),
                             TString::Format("Histogram variable '%s' of species '%s'",
                             vars[0]->GetTitle(), GetTitle()).Data(),
                             vars[0]->getBinning().numBins(),
                             vars[0]->getBinning().lowBound(),
                             vars[0]->getBinning().highBound());

            // fill the histogram
            fTree->Draw(TString::Format("%s>>hist_%s_%s",
                                        vars[0]->GetName(),
                                        vars[0]->GetName(),
                                        GetName()).Data(),
                        fWeightVar);
        }
        else if (fNDim == 2)
        {
            // create the histogram
            fHist = new TH2F(TString::Format("hist_%s_%s_%s",
                                             vars[0]->GetName(),
                                             vars[1]->GetName(),
                                             GetName()).Data(),
                             TString::Format("Histogram variables '%s' and '%s' of species '%s'",
                             vars[0]->GetTitle(), vars[1]->GetTitle(), GetTitle()).Data(),
                             vars[0]->getBinning().numBins(),
                             vars[0]->getBinning().lowBound(),
                             vars[0]->getBinning().highBound(),
                             vars[1]->getBinning().numBins(),
                             vars[1]->getBinning().lowBound(),
                             vars[1]->getBinning().highBound());

            // fill the histogram
            fTree->Draw(TString::Format("%s:%s>>hist_%s_%s_%s",
                                        vars[1]->GetName(),
                                        vars[0]->GetName(),
                                        vars[0]->GetName(),
                                        vars[1]->GetName(),
                                        GetName()).Data(),
                        fWeightVar);
        }
        else if (fNDim == 3)
        {
            // create the histogram
            fHist = new TH3F(TString::Format("hist_%s_%s_%s_%s",
                                             vars[0]->GetName(),
                                             vars[1]->GetName(),
                                             vars[2]->GetName(),
                                             GetName()).Data(),
                             TString::Format("Histogram variables '%s', '%s' and '%s' of species '%s'",
                             vars[0]->GetTitle(), vars[1]->GetTitle(), vars[2]->GetTitle(), GetTitle()).Data(),
                             vars[0]->getBinning().numBins(),
                             vars[0]->getBinning().lowBound(),
                             vars[0]->getBinning().highBound(),
                             vars[1]->getBinning().numBins(),
                             vars[1]->getBinning().lowBound(),
                             vars[1]->getBinning().highBound(),
                             vars[2]->getBinning().numBins(),
                             vars[2]->getBinning().lowBound(),
                             vars[2]->getBinning().highBound());

            // fill the histogram
            fTree->Draw(TString::Format("%s:%s:%s>>hist_%s_%s_%s_%s",
                                        vars[2]->GetName(),
                                        vars[1]->GetName(),
                                        vars[0]->GetName(),
                                        vars[0]->GetName(),
                                        vars[1]->GetName(),
                                        vars[2]->GetName(),
                                        GetName()).Data(),
                        fWeightVar);
        }
        else
        {
            Error("BuildModel", "Cannot convert unbinned input data of dimension %d!", fNDim);
            return;
        }
    }

    // create RooFit histogram
    if (fDataHist) delete fDataHist;
    fDataHist = new RooDataHist(TString::Format("%s_RooFit", fHist->GetName()),
                                TString::Format("%s (RooFit)", fHist->GetTitle()),
                                varSet, RooFit::Import(*fHist));

    // add shift transformation
    RooArgSet varSetTrans;
    if (fNPar)
    {
        // loop over dimensions
        for (Int_t i = 0; i < fNDim; i++)
        {
            // add shift transformation
            sprintf(tmp, "%s - %s", vars[i]->GetName(), GetPar(i)->GetName());
            RooFormulaVar* trans = new RooFormulaVar(TString::Format("Trans_Shift_%s", vars[i]->GetName()).Data(),
                                                     tmp, RooArgSet(*vars[i], *GetPar(i)));
            AddVarTrans(trans);
            varSetTrans.add(*trans);
        }
    }

    // create the model pdf
    if (fPdf) delete fPdf;
    if (fNPar)
        fPdf = new RooHistPdf(GetName(), GetTitle(), varSetTrans, varSet, *fDataHist, 1);
    else
        fPdf = new RooHistPdf(GetName(), GetTitle(), varSet, *fDataHist);
}

