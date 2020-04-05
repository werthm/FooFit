/*************************************************************************
 * Author: Dominik Werthmueller, 2017-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitter                                                          //
//                                                                      //
// Abstract class for fitting multiple species to data.                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooFitter
#define FOOFIT_FFRooFitter

#include <vector>

#include "RooKeysPdf.h"

#include "FFRooFit.h"

class TTree;
class TChain;
class TH1;
class RooAbsReal;
class FFRooFit;
class FFRooFitterSpecies;

class FFRooFitter : public TNamed
{

protected:
    TTree* fTree;                   // unbinned input data
    std::vector<TTree*> fTreeAdd;   // additional unbinned input data
    TH1* fHist;                     // binned input data
    TString fWeightVar;             // name of event weight variable
    FFRooFit* fFitter;              // RooFit fitter
    FFRooModel* fModel;             // total model
    Int_t fNSpec;                   // number of species
    FFRooFitterSpecies** fSpec;     //[fNSpec] array of species

    TString BuildModelName(const Char_t* name);
    TChain* LoadChainSpecies(const Char_t* name, const Char_t* treeLoc);
    Bool_t AddSpeciesModel(const Char_t* name, const Char_t* title,
                           const Char_t* type, FFRooModel* model);
    Bool_t BuildModel();

public:
    FFRooFitter(): TNamed(),
                   fTree(0), fHist(0),
                   fWeightVar(""),
                   fFitter(0),
                   fModel(0),
                   fNSpec(0), fSpec(0) { }
    FFRooFitter(const Char_t* name, const Char_t* title);
    virtual ~FFRooFitter();

    void AddWeightedTree(const Char_t* treeLoc, Double_t weightScale);

    FFRooModel* GetModel() const { return fModel; }

    Int_t GetNSpecies() const { return fNSpec; }
    FFRooFitterSpecies* GetSpecies(Int_t i) const;
    RooRealVar* GetVariable(Int_t i) const;

    void AddSpecies(FFRooFitterSpecies* spec);
    Bool_t AddSpeciesGaussPdf(const Char_t* name, const Char_t* title);
    Bool_t AddSpeciesGaussBifurPdf(const Char_t* name, const Char_t* title);
    Bool_t AddSpeciesLandauPdf(const Char_t* name, const Char_t* title,
                               Bool_t gaussConvol = kFALSE);
    Bool_t AddSpeciesPolPdf(const Char_t* name, const Char_t* title, Int_t nOrder);
    Bool_t AddSpeciesChebychevPdf(const Char_t* name, const Char_t* title, Int_t nOrder);
    Bool_t AddSpeciesExpoPdf(const Char_t* name, const Char_t* title);
    Bool_t AddSpeciesHistPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                             Bool_t gaussConvol = kFALSE, Int_t intOrder = 0);
    Bool_t AddSpeciesHistPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                             RooAbsReal** convolPar, Int_t intOrder = 0);
    Bool_t AddSpeciesHistPdf(const Char_t* name, const Char_t* title, TH1* hist,
                             Bool_t gaussConvol = kFALSE, Int_t intOrder = 0);
    Bool_t AddSpeciesKeysPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                             const Char_t* opt = "a", Double_t rho = 1, Int_t nSigma = 3, Bool_t rotate = kTRUE);
    Bool_t AddSpeciesKeysPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                             Bool_t addShiftPar = kFALSE, RooKeysPdf::Mirror mirror = RooKeysPdf::NoMirror, Double_t rho = 1);

    void SetVariable(Int_t i, const Char_t* name, const Char_t* title,
                     Double_t min, Double_t max, Int_t nbins);
    void SetVariableAutoRange(Int_t i, const Char_t* name, const Char_t* title,
                              Int_t nbins);
    void AddAuxVariable(RooRealVar* aux_var);
    void AddControlVariable(const Char_t* name, const Char_t* title);
    void AddConstraint(FFRooModel* c);
    void SetNChi2PreFit(Int_t n);
    void SetMinimizer(FFRooFit::FFMinimizer_t min);
    void SetMinimizerPreFit(FFRooFit::FFMinimizer_t min);
    void SetFitRange(Double_t min, Double_t max);

    virtual Bool_t Fit(const Char_t* opt = "");

    TCanvas* DrawFit(const Char_t* opt = "", Int_t var = -1);
    TCanvas* DrawCorrelations(const Char_t* opt = "");
    RooPlot* PlotDataAndModel(Int_t var, const Char_t* opt = "");
    TH2* PlotData2D(Int_t var0, Int_t var1);
    TH2* PlotModel2D(Int_t var0, Int_t var1);
    TH1* CreateDataHistogram(Int_t var);

    ClassDef(FFRooFitter, 0)  // Abstract class for species fitting
};

#endif

