/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2016
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooSPlot                                                           //
//                                                                      //
// Class for performing RooFit-based sPlot fits.                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooSPlot
#define FOOFIT_FFRooSPlot

#include "FFRooFitTree.h"

namespace RooStats
{
    class SPlot;
};
class RooRealVar;
class FFRooModel;

class FFRooSPlot : public FFRooFitTree
{

protected:
    Int_t fNSpec;                   // number of species
    FFRooModel** fSpecModel;        //[fNSpec] array of models (objects not owned)
    RooRealVar* fEventID;           // event ID variable
    RooStats::SPlot* fSPlot;        // sPlot object

    Bool_t CheckSpecBounds(Int_t spec, const Char_t* loc) const;
    Bool_t CheckEventID();

public:
    FFRooSPlot() : FFRooFitTree(),
                   fNSpec(0), fSpecModel(0),
                   fEventID(0), fSPlot(0) { }
    FFRooSPlot(TChain* chain, Int_t nVar, Int_t nSpec,
               const Char_t* name = "FFRooSPlot", const Char_t* title = "a FooFit RooFit",
               const Char_t* evIDVar = "event_id", const Char_t* weightVar = 0);
    virtual ~FFRooSPlot();

    Int_t GetNSpecies() const { return fNSpec; }
    FFRooModel** GetSpeciesModels() const { return fSpecModel; }
    FFRooModel* GetSpeciesModel(Int_t i) const { return fSpecModel[i]; }
    const Char_t* GetSpeciesName(Int_t i) const;

    void SetSpeciesModel(Int_t i, FFRooModel* model);
    void SetSpeciesYield(Int_t i, Double_t v);
    void SetSpeciesYield(Int_t i, Double_t v, Double_t min, Double_t max);
    void SetSpeciesYieldLimits(Int_t i, Double_t min, Double_t max);
    void SetSpeciesName(Int_t i, const Char_t* name);

    virtual Bool_t Fit();
    Int_t GetNEvents() const;
    Double_t GetEventID(Int_t event) const;
    Double_t GetSpeciesWeight(Int_t event, Int_t i) const;

    ClassDef(FFRooSPlot, 0)  // perform sPlot fits with RooFit
};

#endif

