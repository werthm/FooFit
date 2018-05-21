/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2018
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitTree                                                         //
//                                                                      //
// Class performing RooFit fits of trees.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooFitTree
#define FOOFIT_FFRooFitTree

#include "FFRooFit.h"

class TTree;

class FFRooFitTree : public FFRooFit
{

protected:
    TTree* fTree;                   // input tree (not owned)
    RooRealVar* fWeights;           // weights variable
    Bool_t fIsBinnedFit;            // binned fit flag

    virtual Bool_t LoadData();

public:
    FFRooFitTree() : FFRooFit(),
                     fTree(0), fWeights(0), fIsBinnedFit(kFALSE) { }
    FFRooFitTree(TTree* tree, Int_t nVar,
                 const Char_t* name = "FFRooFitTree", const Char_t* title = "a FooFit RooFit",
                 const Char_t* weightVar = 0, Bool_t binnedFit = kFALSE);
    virtual ~FFRooFitTree();

    TTree* GetTree() const { return fTree; }

    void SetTree(TTree* tree) { fTree = tree; }

    ClassDef(FFRooFitTree, 0)  // Fit trees using RooFit
};

#endif

