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
                               Bool_t addShiftPar, Int_t intOrder)
    : FFRooModel(name, title, addShiftPar ? hist->GetDimension() : 0)
{
    // Constructor.

    Char_t tmp[256];

    // init members
    fNDim = hist->GetDimension();
    fHist = hist;
    fTree = 0;
    fWeightVar = "";
    fInterpolOrder = intOrder;
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
                               const Char_t* weightVar, Bool_t addShiftPar, Int_t intOrder)
    : FFRooModel(name, title, addShiftPar ? nDim : 0)
{
    // Constructor.

    Char_t tmp[256];

    // init members
    fNDim = nDim;
    fHist = 0;
    fTree = tree;
    fWeightVar = "";
    fInterpolOrder = intOrder;
    if (weightVar)
        fWeightVar = weightVar;
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
    if (fDataHist) delete fDataHist;
}

//______________________________________________________________________________
void FFRooModelHist::DetermineHistoBinning(RooRealVar* var, RooRealVar* par,
                                           Int_t* nBin, Double_t* min, Double_t* max)
{
    // Determine the binning of the histogram used to construct the pdf for
    // the variable 'var' taking into account the parameter 'par'.
    // Return the number of bins and the lower and upper bounds via 'nBin',
    // 'min', and 'max', respectively.

    // calculate the binning
    RooAbsBinning& binning = var->getBinning();
    Double_t binw = binning.averageBinWidth();

    // different binning if shift parameter is present
    if (par)
    {
        // extend range due to shift parameter
        Double_t lmin = TMath::Min(binning.lowBound() - par->getMin(), binning.lowBound() - par->getMax());
        Double_t lmax = TMath::Max(binning.highBound() - par->getMin(), binning.highBound() - par->getMax());
        *min = binning.lowBound() - binw;
        *max = binning.highBound() + binw;

        // extend range to original binning
        while (*min > lmin) *min -= binw;
        while (*max < lmax) *max += binw;
        *nBin = (*max - *min) / binw;
    }
    else
    {
        *min = binning.lowBound() - binw;
        *max = binning.highBound() + binw;
        *nBin = binning.numBins() + 2;
    }
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
            // calculate the binning
            Int_t nbin_0 = 0;
            Double_t min_0 = 0, max_0 = 0;
            if (fNPar)
                DetermineHistoBinning(vars[0], fPar[0], &nbin_0, &min_0, &max_0);
            else
                DetermineHistoBinning(vars[0], 0, &nbin_0, &min_0, &max_0);

            // create the histogram
            fHist = new TH1F(TString::Format("hist_%s_%s",
                                             vars[0]->GetName(),
                                             GetName()).Data(),
                             TString::Format("Histogram variable '%s' of species '%s'",
                             vars[0]->GetTitle(), GetTitle()).Data(),
                             nbin_0, min_0, max_0);

            // fill the histogram
            fTree->Draw(TString::Format("%s>>hist_%s_%s",
                                        vars[0]->GetName(),
                                        vars[0]->GetName(),
                                        GetName()).Data(),
                        fWeightVar.Data());
        }
        else if (fNDim == 2)
        {
            // calculate the binning
            Int_t nbin_0 = 0;
            Int_t nbin_1 = 0;
            Double_t min_0 = 0, max_0 = 0;
            Double_t min_1 = 0, max_1 = 0;
            if (fNPar)
            {
                DetermineHistoBinning(vars[0], fPar[0], &nbin_0, &min_0, &max_0);
                DetermineHistoBinning(vars[1], fPar[1], &nbin_1, &min_1, &max_1);
            }
            else
            {
                DetermineHistoBinning(vars[0], 0, &nbin_0, &min_0, &max_0);
                DetermineHistoBinning(vars[1], 0, &nbin_1, &min_1, &max_1);
            }

            // create the histogram
            fHist = new TH2F(TString::Format("hist_%s_%s_%s",
                                             vars[0]->GetName(),
                                             vars[1]->GetName(),
                                             GetName()).Data(),
                             TString::Format("Histogram variables '%s' and '%s' of species '%s'",
                             vars[0]->GetTitle(), vars[1]->GetTitle(), GetTitle()).Data(),
                             nbin_0, min_0, max_0,
                             nbin_1, min_1, max_1);

            // fill the histogram
            fTree->Draw(TString::Format("%s:%s>>hist_%s_%s_%s",
                                        vars[1]->GetName(),
                                        vars[0]->GetName(),
                                        vars[0]->GetName(),
                                        vars[1]->GetName(),
                                        GetName()).Data(),
                        fWeightVar.Data());
        }
        else if (fNDim == 3)
        {
            // calculate the binning
            Int_t nbin_0 = 0;
            Int_t nbin_1 = 0;
            Int_t nbin_2 = 0;
            Double_t min_0 = 0, max_0 = 0;
            Double_t min_1 = 0, max_1 = 0;
            Double_t min_2 = 0, max_2 = 0;
            if (fNPar)
            {
                DetermineHistoBinning(vars[0], fPar[0], &nbin_0, &min_0, &max_0);
                DetermineHistoBinning(vars[1], fPar[1], &nbin_1, &min_1, &max_1);
                DetermineHistoBinning(vars[2], fPar[2], &nbin_2, &min_2, &max_2);
            }
            else
            {
                DetermineHistoBinning(vars[0], 0, &nbin_0, &min_0, &max_0);
                DetermineHistoBinning(vars[1], 0, &nbin_1, &min_1, &max_1);
                DetermineHistoBinning(vars[2], 0, &nbin_2, &min_2, &max_2);
            }

            // create the histogram
            fHist = new TH3F(TString::Format("hist_%s_%s_%s_%s",
                                             vars[0]->GetName(),
                                             vars[1]->GetName(),
                                             vars[2]->GetName(),
                                             GetName()).Data(),
                             TString::Format("Histogram variables '%s', '%s' and '%s' of species '%s'",
                             vars[0]->GetTitle(), vars[1]->GetTitle(), vars[2]->GetTitle(), GetTitle()).Data(),
                             nbin_0, min_0, max_0,
                             nbin_1, min_1, max_1,
                             nbin_2, min_2, max_2);

            // fill the histogram
            fTree->Draw(TString::Format("%s:%s:%s>>hist_%s_%s_%s_%s",
                                        vars[2]->GetName(),
                                        vars[1]->GetName(),
                                        vars[0]->GetName(),
                                        vars[0]->GetName(),
                                        vars[1]->GetName(),
                                        vars[2]->GetName(),
                                        GetName()).Data(),
                        fWeightVar.Data());
        }
        else
        {
            Error("BuildModel", "Cannot convert unbinned input data of dimension %d!", fNDim);
            return;
        }
    }

    // backup binning of variables
    Int_t vbins[fNDim];
    Double_t vmin[fNDim];
    Double_t vmax[fNDim];
    for (Int_t i = 0; i < fNDim; i++)
    {
        vbins[i] = vars[i]->getBinning().numBins();
        vmin[i] = vars[i]->getBinning().lowBound();
        vmax[i] = vars[i]->getBinning().highBound();
    }

    // extend variables to range of histogram
    TAxis* haxes[3] = { fHist->GetXaxis(), fHist->GetYaxis(), fHist->GetZaxis() };
    for (Int_t i = 0; i < fNDim; i++)
    {
        vars[i]->setBins(haxes[i]->GetNbins());
        vars[i]->setMin(haxes[i]->GetXmin());
        vars[i]->setMax(haxes[i]->GetXmax());
    }

    // create RooFit histogram
    if (fDataHist) delete fDataHist;
    fDataHist = new RooDataHist(TString::Format("%s_RooFit", fHist->GetName()),
                                TString::Format("%s (RooFit)", fHist->GetTitle()),
                                varSet, RooFit::Import(*fHist));

    // restore binning of variables
    for (Int_t i = 0; i < fNDim; i++)
    {
        vars[i]->setBins(vbins[i]);
        vars[i]->setMin(vmin[i]);
        vars[i]->setMax(vmax[i]);
    }

    // add shift transformation
    RooArgSet varSetTrans;
    if (fNPar)
    {
        // loop over dimensions
        for (Int_t i = 0; i < fNDim; i++)
        {
            // add shift transformation
            sprintf(tmp, "%s - %s", vars[i]->GetName(), GetPar(i)->GetName());
            RooFormulaVar* trans = new RooFormulaVar(TString::Format("Trans_Shift_%s_Var_%s", GetName(), vars[i]->GetName()).Data(),
                                                     tmp, RooArgSet(*vars[i], *GetPar(i)));
            AddVarTrans(trans);
            varSetTrans.add(*trans);
        }
    }

    // create the model pdf
    if (fPdf) delete fPdf;
    if (fNPar)
        fPdf = new RooHistPdf(GetName(), GetTitle(), varSetTrans, varSet, *fDataHist, fInterpolOrder);
    else
        fPdf = new RooHistPdf(GetName(), GetTitle(), varSet, *fDataHist, fInterpolOrder);
}

