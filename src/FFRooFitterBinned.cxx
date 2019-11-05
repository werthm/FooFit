/*************************************************************************
 * Author: Dominik Werthmueller, 2018-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitterBinned                                                    //
//                                                                      //
// Class for fitting multiple species to binned data.                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TChain.h"

#include "FFRooFitterBinned.h"
#include "FFRooFitTree.h"
#include "FFFooFit.h"

ClassImp(FFRooFitterBinned)

//______________________________________________________________________________
FFRooFitterBinned::FFRooFitterBinned(TTree* tree, Int_t nVar,
                                     const Char_t* name, const Char_t* title,
                                     const Char_t* weightVar)
    : FFRooFitter(name, title)
{
    // Constructor.

    // init members
    fTree = tree;
    if (weightVar)
        fWeightVar = weightVar;
    fFitter = new FFRooFitTree(fTree, nVar, GetName(), GetTitle(), weightVar, kTRUE);
}

//______________________________________________________________________________
FFRooFitterBinned::FFRooFitterBinned(const Char_t* treeName, const Char_t* treeLoc, Int_t nVar,
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
        fWeightVar = weightVar;
    fFitter = new FFRooFitTree(fTree, nVar, GetName(), GetTitle(), weightVar, kTRUE);
}

