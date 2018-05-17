/*************************************************************************
 * Author: Dominik Werthmueller, 2017-2018
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitterUnbinned                                                  //
//                                                                      //
// Class for fitting multiple species to unbinned data.                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TChain.h"
#include "TLeaf.h"
#include "TH1.h"
#include "TMath.h"
#include "RooRealVar.h"

#include "FFRooFitterUnbinned.h"
#include "FFRooFitterSpecies.h"
#include "FFRooFitTree.h"
#include "FFFooFit.h"
#include "FFRooModelHist.h"
#include "FFRooModelKeys.h"
#include "FFRooModelProd.h"

ClassImp(FFRooFitterUnbinned)

//______________________________________________________________________________
FFRooFitterUnbinned::FFRooFitterUnbinned(TTree* tree, Int_t nVar,
                                         const Char_t* name, const Char_t* title,
                                         const Char_t* weightVar)
    : FFRooFitter(name, title)
{
    // Constructor.

    // init members
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
    fFitter = new FFRooFitTree(fTree, nVar, GetName(), GetTitle(), fWeightVar);
}

//______________________________________________________________________________
FFRooFitterUnbinned::FFRooFitterUnbinned(const Char_t* treeName, const Char_t* treeLoc, Int_t nVar,
                                         const Char_t* name, const Char_t* title,
                                         const Char_t* weightVar)
    : FFRooFitter(name, title)
{
    // Constructor.

    // load tree
    TChain* chain = new TChain(treeName);
    FFFooFit::LoadFilesToChain(treeLoc, chain);

    // init members
    fTree = chain;
    if (weightVar)
    {
        fWeightVar = new Char_t[256];
        strcpy(fWeightVar, weightVar);
    }
    else
    {
        fWeightVar = 0;
    }
    fFitter = new FFRooFitTree(fTree, nVar, GetName(), GetTitle(), fWeightVar);
}

//______________________________________________________________________________
FFRooFitterUnbinned::~FFRooFitterUnbinned()
{
    // Destructor.

    if (fTree) delete fTree;
    if (fWeightVar) delete [] fWeightVar;
}

//______________________________________________________________________________
void FFRooFitterUnbinned::SetVariableAutoRange(Int_t i, const Char_t* name, const Char_t* title,
                                               Int_t nbins)
{
    // Wrapper for FFRooFit::SetVariable() which automatically determines the
    // range of the variable 'i'.

    //
    // look for minimum and maximum variable value in tree
    // (TTree::GetMinimum() is crashing)
    //

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
    fTree->Draw(TString::Format("%s>>h_det_range", name).Data(), fWeightVar);

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
Bool_t FFRooFitterUnbinned::AddSpeciesHistPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                                              Bool_t addShiftPar, Int_t intOrder)
{
    // Add the species with name 'name', title 'title' and tree location 'treeLoc'
    // to the list of species to be fit using a histogram pdf.
    // Add a shift parameter if 'addShiftPar' is kTRUE.
    // The order of the histogram interpolation can be specified via 'intOrder'.
    // Return kTRUE if the species was added, otherwise return kFALSE.

    // models for all fit variables
    FFRooModel* models[fFitter->GetNVariable()];

    // load tree
    TChain* chain = new TChain(fTree->GetName());
    FFFooFit::LoadFilesToChain(treeLoc, chain);

    // check entries
    if (!chain->GetEntries())
    {
        Warning("AddSpeciesHistPdf", "Not adding species %s as no entries were found in tree %s!", name, treeLoc);
        delete chain;
        return kFALSE;
    }

    // loop over fit variables
    Char_t tmp[256] = "";
    for (Int_t i = 0; i < fFitter->GetNVariable(); i++)
    {
        strcat(tmp, fFitter->GetVariable(i)->GetName());
        strcat(tmp, "_");
    }

    // create the model
    FFRooModel* tot_model = new FFRooModelHist(TString::Format("%s%s", tmp, name).Data(),
                                               title, fFitter->GetNVariable(), chain, fWeightVar,
                                               addShiftPar, intOrder);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, tot_model);

    // add species
    AddSpecies(spec);

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFitterUnbinned::AddSpeciesHistPdf(const Char_t* name, const Char_t* title, TH1* hist,
                                              Bool_t addShiftPar, Int_t intOrder)
{
    // Add the species with name 'name', title 'title' and histogram 'hist'
    // to the list of species to be fit using a histogram pdf.
    // Add a shift parameter if 'addShiftPar' is kTRUE.
    // The order of the histogram interpolation can be specified via 'intOrder'.
    // Return kTRUE if the species was added, otherwise return kFALSE.

    // models for all fit variables
    FFRooModel* models[fFitter->GetNVariable()];

    // loop over fit variables
    Char_t tmp[256] = "";
    for (Int_t i = 0; i < fFitter->GetNVariable(); i++)
    {
        strcat(tmp, fFitter->GetVariable(i)->GetName());
        strcat(tmp, "_");
    }

    // create the model
    FFRooModel* tot_model = new FFRooModelHist(TString::Format("%s%s", tmp, name).Data(),
                                               title, hist, addShiftPar, intOrder);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, tot_model);

    // add species
    AddSpecies(spec);

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFitterUnbinned::AddSpeciesKeysPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                                              const Char_t* opt, Double_t rho, Int_t nSigma, Bool_t rotate)
{
    // Add the species with name 'name', title 'title' and tree location 'treeLoc'
    // to the list of species to be fit using a kernel estimation pdf.
    // See RooNDKeysPdf for meaning of parameters 'opt', 'rho', 'nSigma' and 'rotate'.
    // Return kTRUE if the species was added, otherwise return kFALSE.

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
    Char_t tmp[256] = "";
    for (Int_t i = 0; i < fFitter->GetNVariable(); i++)
    {
        strcat(tmp, fFitter->GetVariable(i)->GetName());
        strcat(tmp, "_");
    }

    // create the model
    FFRooModel* tot_model = new FFRooModelKeys(TString::Format("%s%s", tmp, name).Data(),
                                               title, fFitter->GetNVariable(), chain,
                                               opt, rho, nSigma, rotate);

    // create species
    FFRooFitterSpecies* spec = new FFRooFitterSpecies(name, title, tot_model);

    // add species
    AddSpecies(spec);

    return kTRUE;
}

