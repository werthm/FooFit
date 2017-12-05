/*************************************************************************
 * Author: Dominik Werthmueller, 2017
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

#include "FFRooFit.h"

class FFRooFit;
class FFRooFitterSpecies;

class FFRooFitter : public TNamed
{

protected:
    FFRooFit* fFitter;              // RooFit fitter
    FFRooModel* fModel;             // total model
    Int_t fNSpec;                   // number of species
    FFRooFitterSpecies** fSpec;     //[fNSpec] array of species

    Bool_t BuildModel();

public:
    FFRooFitter(): TNamed(),
                   fFitter(0),
                   fModel(0),
                   fNSpec(0), fSpec(0) { }
    FFRooFitter(const Char_t* name, const Char_t* title);
    virtual ~FFRooFitter();

    Int_t GetNSpecies() const { return fNSpec; }
    FFRooFitterSpecies* GetSpecies(Int_t i) const;

    void AddSpecies(FFRooFitterSpecies* spec);
    void SetVariable(Int_t i, const Char_t* name, const Char_t* title,
                     Double_t min, Double_t max, Int_t nbins);
    void AddAuxVariable(RooRealVar* aux_var);
    void AddControlVariable(const Char_t* name, const Char_t* title);
    void AddConstraint(FFRooModel* c);
    void SetNChi2PreFit(Int_t n);
    void SetMinimizer(FFRooFit::FFMinimizer_t min);

    virtual Bool_t Fit(const Char_t* opt = "");

    TCanvas* DrawFit(const Char_t* opt = "", Int_t var = -1);
    TCanvas* DrawCorrelations(const Char_t* opt = "");
    RooPlot* PlotDataAndModel(Int_t var, const Char_t* opt = "");
    TH2* PlotData2D(Int_t var0, Int_t var1);
    TH2* PlotModel2D(Int_t var0, Int_t var1);

    ClassDef(FFRooFitter, 0)  // Abstract class for species fitting
};

#endif

