/*************************************************************************
 * Author: Dominik Werthmueller, 2017-2019
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
    : FFRooFitterUnbinned(tree, nVar, name, title, weightVar)
{
    // Constructor.

    // replace fitter created in FFRooFitterUnbinned::FFRooFitterUnbinned()
    delete fFitter;
    fFitter = new FFRooSPlot(fTree, nVar, nSpec, GetName(), GetTitle(), evIDVar, fWeightVar);
}

//______________________________________________________________________________
FFRooFitterSPlot::FFRooFitterSPlot(const Char_t* treeName, const Char_t* treeLoc,
                                   Int_t nVar, Int_t nSpec,
                                   const Char_t* name, const Char_t* title,
                                   const Char_t* evIDVar, const Char_t* weightVar)
    : FFRooFitterUnbinned(treeName, treeLoc, nVar, name, title, weightVar)
{
    // Constructor.

    // replace fitter created in FFRooFitterUnbinned::FFRooFitterUnbinned()
    delete fFitter;
    fFitter = new FFRooSPlot(fTree, nVar, nSpec, GetName(), GetTitle(), evIDVar, fWeightVar);
}

//______________________________________________________________________________
Double_t FFRooFitterSPlot::GetSpeciesWeight(Int_t event, Int_t i) const
{
    // Wrapper for FFRooSPlot::GetSpeciesWeight().

    if (fFitter)
        return ((FFRooSPlot*)fFitter)->GetSpeciesWeight(event, i);
    else
        Error("GetSpeciesWeight", "Fitter not created yet!");

    return 0;
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

