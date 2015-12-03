/*************************************************************************
 * Author: Dominik Werthmueller, 2015
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
    RooRealVar* fEventID;           // event id variable
    RooStats::SPlot* fSPlot;        // sPlot object

    Bool_t CheckSpecBounds(Int_t spec, const Char_t* loc) const;
    Bool_t CheckEventID();

    static const Char_t fgBranchEventID[];

public:
    FFRooSPlot() : FFRooFitTree(),
                   fNSpec(0), fSpecModel(0),
                   fEventID(0), fSPlot(0) { }
    FFRooSPlot(TChain* chain, Int_t nVar, Int_t nSpec);
    virtual ~FFRooSPlot();

    RooStats::SPlot* GetSPlot() const { return fSPlot; }

    void SetSpeciesModel(Int_t i, FFRooModel* model);
    void SetSpeciesYield(Int_t i, Double_t v);
    void SetSpeciesYield(Int_t i, Double_t v, Double_t min, Double_t max);
    void SetSpeciesYieldLimits(Int_t i, Double_t min, Double_t max);
    void SetSpeciesName(Int_t i, const Char_t* name);

    virtual Bool_t Fit();

    ClassDef(FFRooSPlot, 0)  // perform sPlot fits with RooFit
};

#endif

