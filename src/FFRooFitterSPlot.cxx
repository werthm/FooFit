/*************************************************************************
 * Author: Dominik Werthmueller, 2017
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitterSPlot                                                     //
//                                                                      //
// Class for fitting multiple species to data in a tree and deriving    //
// event weights using sPlot.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "FFRooFitterSPlot.h"
#include "FFRooSPlot.h"
#include "FFRooFitterSpecies.h"

ClassImp(FFRooFitterSPlot)

//______________________________________________________________________________
FFRooFitterSPlot::FFRooFitterSPlot(TTree* tree,
                                   Int_t nVar, Int_t nSpec,
                                   const Char_t* name, const Char_t* title,
                                   const Char_t* evIDVar, const Char_t* weightVar)
    : FFRooFitterTree(tree, nVar, name, title, weightVar)
{
    // Constructor.

    // replace fitter created in FFRooFitterTree::FFRooFitterTree()
    delete fFitter;
    fFitter = new FFRooSPlot(fTree, nVar, nSpec, GetName(), GetTitle(), evIDVar, fWeightVar);
}

//______________________________________________________________________________
FFRooFitterSPlot::FFRooFitterSPlot(const Char_t* treeName, const Char_t* treeLoc,
                                   Int_t nVar, Int_t nSpec,
                                   const Char_t* name, const Char_t* title,
                                   const Char_t* evIDVar, const Char_t* weightVar)
    : FFRooFitterTree(treeName, treeLoc, nVar, name, title, weightVar)
{
    // Constructor.

    // replace fitter created in FFRooFitterTree::FFRooFitterTree()
    delete fFitter;
    fFitter = new FFRooSPlot(fTree, nVar, nSpec, GetName(), GetTitle(), evIDVar, fWeightVar);
}

//______________________________________________________________________________
Bool_t FFRooFitterSPlot::Fit(const Char_t* opt)
{
    // Perform the fit.

    // cast fitter
    FFRooSPlot* splot = (FFRooSPlot*)fFitter;

    // configure the species
    for (Int_t i = 0; i < fNSpec; i++)
    {
        splot->SetSpeciesModel(i, fSpec[i]->GetModel());
        splot->SetSpeciesName(i, fSpec[i]->GetName());
        splot->SetSpeciesYield(i, fSpec[i]->GetYieldInit(),
                                  fSpec[i]->GetYieldMin(),
                                  fSpec[i]->GetYieldMax());
    }

    // perform the fit
    Bool_t res = splot->Fit(opt);

    // copy yield parameters
    for (Int_t i = 0; i < fNSpec; i++)
    {
        fSpec[i]->SetYieldFit(splot->GetSpeciesYield(i));
        fSpec[i]->SetYieldFitError(splot->GetSpeciesYieldError(i));
        fSpec[i]->UpdateModelParameters();
    }

    return res;
}

//______________________________________________________________________________
TTree* FFRooFitterSPlot::GetSpeciesWeightsTree(const Char_t* name,
                                               Int_t nSpec, Int_t* spec) const
{
    // Wrapper for FFRooSPlot::GetSpeciesWeightsTree().

    if (fFitter)
        return ((FFRooSPlot*)fFitter)->GetSpeciesWeightsTree(name, nSpec, spec);
    else
        Error("GetSpeciesWeightsTree", "Fitter not created yet!");

    return 0;
}

