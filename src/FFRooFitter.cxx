/*************************************************************************
 * Author: Dominik Werthmueller, 2017-2018
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitter                                                          //
//                                                                      //
// Abstract class for fitting multiple species to data.                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TChain.h"
#include "TLeaf.h"
#include "TMath.h"
#include "TH1.h"
#include "RooRealVar.h"

#include "FFRooFitter.h"
#include "FFRooModelHist.h"
#include "FFRooModelSum.h"
#include "FFRooModelKeys.h"
#include "FFRooModelGauss.h"
#include "FFRooModelPol.h"
#include "FFRooModelExpo.h"
#include "FFRooFitterSpecies.h"
#include "FFFooFit.h"

ClassImp(FFRooFitter)

//______________________________________________________________________________
FFRooFitter::FFRooFitter(const Char_t* name, const Char_t* title)
    : TNamed(name, title)
{
    // Constructor.

    // init members
    fTree = 0;
    fHist = 0;
    fWeightVar = "";
    fFitter = 0;
    fModel = 0;
    fNSpec = 0;
    fSpec = 0;
}

//______________________________________________________________________________
FFRooFitter::~FFRooFitter()
{
    // Destructor.

    if (fTree) delete fTree;
    if (fHist) delete fHist;
    if (fFitter) delete fFitter;
    if (fModel) delete fModel;
    if (fSpec)
    {
        for (Int_t i = 0; i < fNSpec; i++)
            if (fSpec[i]) delete fSpec[i];
        delete [] fSpec;
    }
}

//______________________________________________________________________________
void FFRooFitter::AddSpecies(FFRooFitterSpecies* spec)
{
    // Add the species 'spec' to the list of species to fit.
    // Note: The object is not copied into this class, only a pointer is set.

    // backup old list
    FFRooFitterSpecies** old = fSpec;

    // create new list
    fSpec = new FFRooFitterSpecies*[fNSpec+1];
    for (Int_t i = 0; i < fNSpec; i++) fSpec[i] = old[i];

    // add new element
    fSpec[fNSpec] = spec;
    fNSpec++;

    // destroy old list
    if (old) delete [] old;
}

//______________________________________________________________________________
FFRooFitterSpecies* FFRooFitter::GetSpecies(Int_t i) const
{
    // Return the species with index 'i'.

    if (i >= 0 && i < fNSpec)
        return fSpec[i];
    else
        return 0;
}

//______________________________________________________________________________
RooRealVar* FFRooFitter::GetVariable(Int_t i) const
{
    // Wrapper for FFRooFit::GetVariable(Int_t i).

    if (fFitter)
        return fFitter->GetVariable(i);
    else
    {
        Error("SetVariable", "Fitter not created yet!");
        return 0;
    }
}

//______________________________________________________________________________
Bool_t FFRooFitter::AddSpeciesGaussPdf(const Char_t* name, const Char_t* title)
{
    // Add the species with name 'name' and title 'title' to the list of species
    // to be fit using a Gaussian pdf.
    // Return kTRUE if the species was added, otherwise return kFALSE.

    // loop over fit variables
    TString tmp;
    for (Int_t i = 0; i < fFitter->GetNVariable(); i++)
    {
        tmp += fFitter->GetVariable(i)->GetName();
        tmp += "_";
    }

    // create the model
    FFRooModel* model = new FFRooModelGauss(TString::Format("%s%s", tmp.Data(), name).Data(), title);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, model);

    // add species
    AddSpecies(spec);

    // user info
    Info("AddSpeciesGaussPdf", "Added species '%s' with Gaussian pdf", title);

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFitter::AddSpeciesPolPdf(const Char_t* name, const Char_t* title, Int_t nOrder)
{
    // Add the species with name 'name' and title 'title' to the list of species
    // to be fit using a polynomial pdf of order 'nOrder'.
    // Return kTRUE if the species was added, otherwise return kFALSE.

    // loop over fit variables
    TString tmp;
    for (Int_t i = 0; i < fFitter->GetNVariable(); i++)
    {
        tmp += fFitter->GetVariable(i)->GetName();
        tmp += "_";
    }

    // create the model
    FFRooModel* model = new FFRooModelPol(TString::Format("%s%s", tmp.Data(), name).Data(), title,
                                          nOrder);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, model);

    // add species
    AddSpecies(spec);

    // user info
    Info("AddSpeciesPolPdf", "Added species '%s' with polynomial pdf", title);

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFitter::AddSpeciesExpoPdf(const Char_t* name, const Char_t* title)
{
    // Add the species with name 'name' and title 'title' to the list of species
    // to be fit using an exponential pdf.
    // Return kTRUE if the species was added, otherwise return kFALSE.

    // loop over fit variables
    TString tmp;
    for (Int_t i = 0; i < fFitter->GetNVariable(); i++)
    {
        tmp += fFitter->GetVariable(i)->GetName();
        tmp += "_";
    }

    // create the model
    FFRooModel* model = new FFRooModelExpo(TString::Format("%s%s", tmp.Data(), name).Data(), title);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, model);

    // add species
    AddSpecies(spec);

    // user info
    Info("AddSpeciesExpoPdf", "Added species '%s' with exponential pdf", title);

    return kTRUE;
}

//______________________________________________________________________________
TChain* FFRooFitter::AddSpeciesHistPdfCommon(const Char_t* name, const Char_t* treeLoc,
                                             TString& outName)
{
    // Prepare adding a histogram-based fit species.

    // check if unbinned data is present
    if (!fTree)
    {
        Error("AddSpeciesHistPdfCommon", "Unbinned input data (tree) needed to extract tree name!");
        return 0;
    }

    // load tree
    TChain* chain = new TChain(fTree->GetName());
    FFFooFit::LoadFilesToChain(treeLoc, chain);

    // check entries
    if (!chain->GetEntries())
    {
        Warning("AddSpeciesHistPdfCommon", "Not adding species %s as no entries were found in tree %s!", name, treeLoc);
        delete chain;
        return 0;
    }

    // loop over fit variables and construct name
    for (Int_t i = 0; i < fFitter->GetNVariable(); i++)
    {
        outName += fFitter->GetVariable(i)->GetName();
        outName += "_";
    }

    return chain;
}

//______________________________________________________________________________
Bool_t FFRooFitter::AddSpeciesHistPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                                      Bool_t addShiftPar, Int_t intOrder)
{
    // Add the species with name 'name', title 'title' and tree location 'treeLoc'
    // to the list of species to be fit using a histogram pdf.
    // Add a shift parameter if 'addShiftPar' is kTRUE.
    // The order of the histogram interpolation can be specified via 'intOrder'.
    // Return kTRUE if the species was added, otherwise return kFALSE.

    // call common method
    TString tmp;
    TChain* chain = AddSpeciesHistPdfCommon(name, treeLoc, tmp);
    if (!chain) return kFALSE;

    // create the model
    FFRooModel* tot_model = new FFRooModelHist(TString::Format("%s%s", tmp.Data(), name).Data(),
                                               title, fFitter->GetNVariable(), chain,
                                               fWeightVar == "" ? 0 : fWeightVar.Data(),
                                               addShiftPar, intOrder);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, tot_model);

    // add species
    AddSpecies(spec);

    // user info
    Info("AddSpeciesHistPdf", "Added species '%s' with histogram pdf", title);

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFitter::AddSpeciesHistPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                                      RooAbsReal** shiftPar, Int_t intOrder)
{
    // Add the species with name 'name', title 'title' and tree location 'treeLoc'
    // to the list of species to be fit using a histogram pdf.
    // Use the parameters 'shiftPar' as shift parameters.
    // The order of the histogram interpolation can be specified via 'intOrder'.
    // Return kTRUE if the species was added, otherwise return kFALSE.

    // call common method
    TString tmp;
    TChain* chain = AddSpeciesHistPdfCommon(name, treeLoc, tmp);
    if (!chain) return kFALSE;

    // create the model
    FFRooModel* tot_model = new FFRooModelHist(TString::Format("%s%s", tmp.Data(), name).Data(),
                                               title, fFitter->GetNVariable(), chain,
                                               shiftPar,
                                               fWeightVar == "" ? 0 : fWeightVar.Data(),
                                               intOrder);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, tot_model);

    // add species
    AddSpecies(spec);

    // user info
    Info("AddSpeciesHistPdf", "Added species '%s' with histogram pdf", title);

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFitter::AddSpeciesHistPdf(const Char_t* name, const Char_t* title, TH1* hist,
                                      Bool_t addShiftPar, Int_t intOrder)
{
    // Add the species with name 'name', title 'title' and histogram 'hist'
    // to the list of species to be fit using a histogram pdf.
    // Add a shift parameter if 'addShiftPar' is kTRUE.
    // The order of the histogram interpolation can be specified via 'intOrder'.
    // Return kTRUE if the species was added, otherwise return kFALSE.

    // loop over fit variables
    TString tmp;
    for (Int_t i = 0; i < fFitter->GetNVariable(); i++)
    {
        tmp += fFitter->GetVariable(i)->GetName();
        tmp += "_";
    }

    // create the model
    FFRooModel* tot_model = new FFRooModelHist(TString::Format("%s%s", tmp.Data(), name).Data(),
                                               title, hist, addShiftPar, intOrder);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, tot_model);

    // add species
    AddSpecies(spec);

    // user info
    Info("AddSpeciesHistPdf", "Added species '%s' with histogram pdf", title);

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFitter::AddSpeciesKeysPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                                      const Char_t* opt, Double_t rho, Int_t nSigma, Bool_t rotate)
{
    // Add the species with name 'name', title 'title' and tree location 'treeLoc'
    // to the list of species to be fit using a kernel estimation pdf.
    // See RooNDKeysPdf for meaning of parameters 'opt', 'rho', 'nSigma' and 'rotate'.
    // Return kTRUE if the species was added, otherwise return kFALSE.

    // check if unbinned data is present
    if (!fTree)
    {
        Error("AddSpeciesKeysPdf", "Unbinned input data (tree) needed to extract tree name!");
        return kFALSE;
    }

    // load tree
    TChain* chain = new TChain(fTree->GetName());
    FFFooFit::LoadFilesToChain(treeLoc, chain);

    // check entries
    if (!chain->GetEntries())
    {
        Warning("AddSpeciesKeysPdf", "Not adding species %s as no entries were found in tree %s!", name, treeLoc);
        delete chain;
        return kFALSE;
    }

    // loop over fit variables
    TString tmp;
    for (Int_t i = 0; i < fFitter->GetNVariable(); i++)
    {
        tmp += fFitter->GetVariable(i)->GetName();
        tmp += "_";
    }

    // create the model
    FFRooModel* tot_model = new FFRooModelKeys(TString::Format("%s%s", tmp.Data(), name).Data(),
                                               title, fFitter->GetNVariable(), chain,
                                               opt, rho, nSigma, rotate);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, tot_model);

    // add species
    AddSpecies(spec);

    // user info
    Info("AddSpeciesKeysPdf", "Added species '%s' with keys pdf", title);

    return kTRUE;
}

//______________________________________________________________________________
void FFRooFitter::SetVariable(Int_t i, const Char_t* name, const Char_t* title,
                              Double_t min, Double_t max, Int_t nbins)
{
    // Wrapper for FFRooFit::SetVariable().

    if (fFitter)
        fFitter->SetVariable(i, name, title, min, max, nbins);
    else
        Error("SetVariable", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::SetVariableAutoRange(Int_t i, const Char_t* name, const Char_t* title,
                                       Int_t nbins)
{
    // Wrapper for FFRooFit::SetVariable() which automatically determines the
    // range of the variable 'i'.

    //
    // look for minimum and maximum variable value in tree
    // (TTree::GetMinimum() is crashing)
    //

    // check if unbinned data is present
    if (!fTree)
    {
        Error("SetVariableAutoRange", "No unbinned input data (tree) was specified!");
        return;
    }

    // try to find the leaf
    TLeaf* leaf = fTree->FindLeaf(name);
    if (!leaf)
    {
        Error("Error", "Variable '%s' not found in data tree!", name);
        return;
    }

    // do not read other branches
    fTree->SetBranchStatus("*", 0);
    fTree->SetBranchStatus(name, 1);

    // loop over events
    Double_t min = 1e30;
    Double_t max = 1e-30;
    for (Long64_t i = 0; i < fTree->GetEntries(); i++)
    {
        // read entry
        fTree->GetEntry(i);

        // update leaf
        leaf = fTree->FindLeaf(name);

        // get minimum/maximum
        min = TMath::Min(min, leaf->GetValue(0));
        max = TMath::Max(max, leaf->GetValue(0));
    }

    // reset branch addresses
    fTree->ResetBranchAddresses();
    fTree->SetBranchStatus("*", 1);

    // create histogram of data
    TH1* h = new TH1F("h_det_range", "h_det_range", nbins, min, max);
    fTree->Draw(TString::Format("%s>>h_det_range", name).Data(), fWeightVar == "" ? 0 : fWeightVar.Data());

    // get bin with maximum
    Int_t maxBin = h->GetMaximumBin();

    // determine minimum
    for (Int_t i = maxBin-1; i > 1; i--)
    {
        // calculate integrals
        Double_t int_low = h->Integral(1, i);
        Double_t int_tot = h->Integral(1, h->GetNbinsX());

        // leave if ratio is below threshold
        if (int_low/int_tot < 0.005)
        {
            min = h->GetBinCenter(i);
            break;
        }
    }

    // determine maximum
    for (Int_t i = maxBin+1; i <= h->GetNbinsX(); i++)
    {
        if (h->GetBinContent(i) == 0)
        {
            max = h->GetBinCenter(i-1);
            break;
        }
    }

    // clean-up
    delete h;

    // round values
    min = TMath::Ceil(min);
    max = TMath::Floor(max);

    // set variable
    fFitter->SetVariable(i, name, title, min, max, nbins);
}

//______________________________________________________________________________
void FFRooFitter::AddAuxVariable(RooRealVar* aux_var)
{
    // Wrapper for FFRooFit::AddAuxVariable().

    if (fFitter)
        fFitter->AddAuxVariable(aux_var);
    else
        Error("AddAuxVariable", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::AddControlVariable(const Char_t* name, const Char_t* title)
{
    // Wrapper for FFRooFit::AddControlVariable().

    if (fFitter)
        fFitter->AddControlVariable(name, title);
    else
        Error("AddControlVariable", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::AddConstraint(FFRooModel* c)
{
    // Wrapper for FFRooFit::AddConstraint().

    if (fFitter)
        fFitter->AddConstraint(c);
    else
        Error("AddConstraint", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::SetNChi2PreFit(Int_t n)
{
    // Wrapper for FFRooFit::SetNChi2PreFit().

    if (fFitter)
        fFitter->SetNChi2PreFit(n);
    else
        Error("SetNChi2PreFit", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::SetMinimizer(FFRooFit::FFMinimizer_t min)
{
    // Wrapper for FFRooFit::SetMinimizer().

    if (fFitter)
        fFitter->SetMinimizer(min);
    else
        Error("SetMinimizer", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::SetMinimizerPreFit(FFRooFit::FFMinimizer_t min)
{
    // Wrapper for FFRooFit::SetMinimizerPreFit().

    if (fFitter)
        fFitter->SetMinimizerPreFit(min);
    else
        Error("SetMinimizerPreFit", "Fitter not created yet!");
}

//______________________________________________________________________________
TCanvas* FFRooFitter::DrawFit(const Char_t* opt, Int_t var)
{
    // Wrapper for FFRooFit::DrawFit().

    if (fFitter)
        return fFitter->DrawFit(opt, var);
    else
        Error("DrawFit", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
TCanvas* FFRooFitter::DrawCorrelations(const Char_t* opt)
{
    // Wrapper for FFRooFit::DrawCorrelations().

    if (fFitter)
        return fFitter->DrawCorrelations(opt);
    else
        Error("DrawCorrelations", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
RooPlot* FFRooFitter::PlotDataAndModel(Int_t var, const Char_t* opt)
{
    // Wrapper for FFRooFit::PlotDataAndModel().

    if (fFitter)
        return fFitter->PlotDataAndModel(var, opt);
    else
        Error("PlotDataAndModel", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
TH2* FFRooFitter::PlotData2D(Int_t var0, Int_t var1)
{
    // Wrapper for FFRooFit::PlotData2D().

    if (fFitter)
        return fFitter->PlotData2D(var0, var1);
    else
        Error("PlotData2D", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
TH2* FFRooFitter::PlotModel2D(Int_t var0, Int_t var1)
{
    // Wrapper for FFRooFit::PlotModel2D().

    if (fFitter)
        return fFitter->PlotModel2D(var0, var1);
    else
        Error("PlotModel2D", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
Bool_t FFRooFitter::BuildModel()
{
    // Build the total model.
    // Return kFALSE if an error occurred, otherwise kTRUE

    if (fFitter)
    {
        // create total model
        fModel = new FFRooModelSum("total_model", "total model", fNSpec);

        // configure models of all species
        for (Int_t i = 0; i < fNSpec; i++)
        {
            // add model
            ((FFRooModelSum*)fModel)->SetModel(i, fSpec[i]->GetModel());

            // parameter name and title
            fModel->SetParName(i, TString::Format("Yield_%s", fSpec[i]->GetName()));
            fModel->SetParTitle(i, TString::Format("Yield of species '%s'", fSpec[i]->GetTitle()));

            // configure yield parameter
            fModel->SetParameter(i,
                                 fSpec[i]->GetYieldInit(),
                                 fSpec[i]->GetYieldMin(),
                                 fSpec[i]->GetYieldMax());

            // set yield Gaussian constraint
            if (!fModel->IsParConstant(i) && fSpec[i]->GetYieldConstrGSigma() != 0)
                fModel->AddParConstrGauss(i,
                                          fSpec[i]->GetYieldConstrGMean(),
                                          fSpec[i]->GetYieldConstrGSigma());
        }

        // set total model
        fFitter->SetModel(fModel);

        return kTRUE;
    }
    else
    {
        Error("BuildModel", "Fitter not created yet!");
        return kFALSE;
    }
}

//______________________________________________________________________________
Bool_t FFRooFitter::Fit(const Char_t* opt)
{
    // Perform the fit.

    // build the total model
    if (!BuildModel())
    {
        Error("Fit", "An error occurred while building the fit model!");
        return kFALSE;
    }

    // perform the fit
    Bool_t res = fFitter->Fit(opt);

    // copy yield parameters
    for (Int_t i = 0; i < fNSpec; i++)
    {
        fSpec[i]->SetYieldFit(fModel->GetParameter(i));
        fSpec[i]->SetYieldFitError(fModel->GetParError(i));
        fSpec[i]->UpdateModelParameters();
    }

    return res;
}

