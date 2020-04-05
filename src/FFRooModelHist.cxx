/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2019
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
#include "RooGaussModel.h"
#include "RooFFTConvPdf.h"

#include "FFRooModelHist.h"

ClassImp(FFRooModelHist)

//______________________________________________________________________________
FFRooModelHist::FFRooModelHist(const Char_t* name, const Char_t* title, TH1* hist,
                               Bool_t gaussConvol, Int_t intOrder)
    : FFRooModel(name, title, gaussConvol ? hist->GetDimension() * 2 : 0)
{
    // Constructor.

    // init members
    fNDim = hist->GetDimension();
    fHist = hist;
    fTree = 0;
    fWeightVar = "";
    fInterpolOrder = intOrder;
    fDataHist = 0;
    fIsConvol = gaussConvol;
    if (fIsConvol)
        AddGaussConvolPars();
}

//______________________________________________________________________________
FFRooModelHist::FFRooModelHist(const Char_t* name, const Char_t* title, Int_t nDim, TTree* tree,
                               const Char_t* weightVar, Bool_t gaussConvol, Int_t intOrder)
    : FFRooModel(name, title, gaussConvol ? nDim * 2 : 0)
{
    // Constructor.

    // init members
    fNDim = nDim;
    fHist = 0;
    fTree = tree;
    fWeightVar = "";
    fInterpolOrder = intOrder;
    if (weightVar)
        fWeightVar = weightVar;
    fDataHist = 0;
    fIsConvol = gaussConvol;
    if (fIsConvol)
        AddGaussConvolPars();
}

//______________________________________________________________________________
FFRooModelHist::FFRooModelHist(const Char_t* name, const Char_t* title, Int_t nDim, TTree* tree,
                               RooAbsReal** convolPar, const Char_t* weightVar, Int_t intOrder)
    : FFRooModel(name, title, nDim * 2)
{
    // Constructor.

    // init members
    fNDim = nDim;
    fHist = 0;
    fTree = tree;
    fWeightVar = "";
    fInterpolOrder = intOrder;
    if (weightVar)
        fWeightVar = weightVar;
    fDataHist = 0;
    fIsConvol = kTRUE;

    // set Gaussian convolution parameters
    for (Int_t i = 0; i < fNPar; i++)
        fPar[i] = convolPar[i];
}

//______________________________________________________________________________
FFRooModelHist::~FFRooModelHist()
{
    // Destructor.

    if (fHist)
        delete fHist;
    if (fTree)
        delete fTree;
    if (fDataHist)
        delete fDataHist;
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

    // different binning if convolution is used
    if (fIsConvol)
    {
        // extend range due to bias parameter
        Double_t lmin = TMath::Min(binning.lowBound(), TMath::Min(binning.lowBound() - par->getMin(), binning.lowBound() - par->getMax()));
        Double_t lmax = TMath::Max(binning.highBound(), TMath::Max(binning.highBound() - par->getMin(), binning.highBound() - par->getMax()));
        *min = binning.lowBound() - binw;
        *max = binning.highBound() + binw;

        // extend range to original binning
        while (*min > lmin)
            *min -= binw;
        while (*max < lmax)
            *max += binw;
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
void FFRooModelHist::AddGaussConvolPars()
{
    // Init the parameters for the Gaussian convolution.

    // loop over dimensions
    for (Int_t i = 0; i < fNDim; i++)
    {
        // add convolution parameters
        TString tmp;
        tmp = TString::Format("%s_%d_Conv_GMean", GetName(), i);
        AddParameter(2*i, tmp.Data(), tmp.Data());
        tmp = TString::Format("%s_%d_Conv_GSigma", GetName(), i);
        AddParameter(2*i+1, tmp.Data(), tmp.Data());
    }
}

//______________________________________________________________________________
void FFRooModelHist::BuildModel(RooAbsReal** vars)
{
    // Build the model using the variables 'vars'.

    // prepare variable set
    RooArgSet varSet;
    for (Int_t i = 0; i < fNDim; i++)
        varSet.add(*vars[i]);

    // check if variables can be down-casted
    for (Int_t i = 0; i < fNDim; i++)
    {
        if (!vars[i]->InheritsFrom("RooRealVar"))
        {
            Error("BuildModel", "Variable '%s' is not of type RooRealVar!", vars[i]->GetName());
            return;
        }
    }

    // check if parameters can be down-casted
    for (Int_t i = 0; i < fNPar; i++)
    {
        if (!fPar[i]->InheritsFrom("RooRealVar"))
        {
            Error("BuildModel", "Parameter '%s' is not of type RooRealVar!", fPar[i]->GetName());
            return;
        }
    }

    // create binned input data
    if (!fHist && fTree)
    {
        // check dimension
        if (fNDim == 1)
        {
            // calculate the binning
            Int_t nbin_0 = 0;
            Double_t min_0 = 0, max_0 = 0;
            if (fIsConvol)
                DetermineHistoBinning((RooRealVar*)vars[0], (RooRealVar*)fPar[0], &nbin_0, &min_0, &max_0);
            else
                DetermineHistoBinning((RooRealVar*)vars[0], 0, &nbin_0, &min_0, &max_0);

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
            if (fIsConvol)
            {
                DetermineHistoBinning((RooRealVar*)vars[0], (RooRealVar*)fPar[0], &nbin_0, &min_0, &max_0);
                DetermineHistoBinning((RooRealVar*)vars[1], (RooRealVar*)fPar[2], &nbin_1, &min_1, &max_1);
            }
            else
            {
                DetermineHistoBinning((RooRealVar*)vars[0], 0, &nbin_0, &min_0, &max_0);
                DetermineHistoBinning((RooRealVar*)vars[1], 0, &nbin_1, &min_1, &max_1);
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
            if (fIsConvol)
            {
                DetermineHistoBinning((RooRealVar*)vars[0], (RooRealVar*)fPar[0], &nbin_0, &min_0, &max_0);
                DetermineHistoBinning((RooRealVar*)vars[1], (RooRealVar*)fPar[2], &nbin_1, &min_1, &max_1);
                DetermineHistoBinning((RooRealVar*)vars[2], (RooRealVar*)fPar[4], &nbin_2, &min_2, &max_2);
            }
            else
            {
                DetermineHistoBinning((RooRealVar*)vars[0], 0, &nbin_0, &min_0, &max_0);
                DetermineHistoBinning((RooRealVar*)vars[1], 0, &nbin_1, &min_1, &max_1);
                DetermineHistoBinning((RooRealVar*)vars[2], 0, &nbin_2, &min_2, &max_2);
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
        vbins[i] = ((RooRealVar*)vars[i])->getBinning().numBins();
        vmin[i] = ((RooRealVar*)vars[i])->getBinning().lowBound();
        vmax[i] = ((RooRealVar*)vars[i])->getBinning().highBound();
    }

    // extend variables to range of histogram
    TAxis* haxes[3] = { fHist->GetXaxis(), fHist->GetYaxis(), fHist->GetZaxis() };
    for (Int_t i = 0; i < fNDim; i++)
    {
        ((RooRealVar*)vars[i])->setBins(haxes[i]->GetNbins());
        ((RooRealVar*)vars[i])->setMin(haxes[i]->GetXmin());
        ((RooRealVar*)vars[i])->setMax(haxes[i]->GetXmax());
    }

    // create RooFit histogram
    if (fDataHist) delete fDataHist;
    fDataHist = new RooDataHist(TString::Format("%s_RooFit", fHist->GetName()),
                                TString::Format("%s (RooFit)", fHist->GetTitle()),
                                varSet, RooFit::Import(*fHist));

    // restore binning of variables
    for (Int_t i = 0; i < fNDim; i++)
    {
        ((RooRealVar*)vars[i])->setBins(vbins[i]);
        ((RooRealVar*)vars[i])->setMin(vmin[i]);
        ((RooRealVar*)vars[i])->setMax(vmax[i]);
    }

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
        fPdfIntr = new RooHistPdf(tmp.Data(), tmp.Data(), varSet, *fDataHist, fInterpolOrder);
        tmp = TString::Format("%s_Conv_Gauss", GetName());
        fPdfConv = new RooGaussModel(tmp.Data(), tmp.Data(), *((RooRealVar*)vars[0]), *fPar[0], *fPar[1]);
        ((RooRealVar*)vars[0])->setBins(10000, "cache");
        fPdf = new RooFFTConvPdf(GetName(), GetTitle(), *((RooRealVar*)vars[0]), *fPdfIntr, *fPdfConv);
    }
    else
    {
        // create pdf
        fPdf = new RooHistPdf(GetName(), GetTitle(), varSet, *fDataHist, fInterpolOrder);
    }
}

