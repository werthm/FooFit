/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2016
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

    virtual Bool_t LoadData();

public:
    FFRooFitTree() : FFRooFit(),
                     fTree(0), fWeights(0) { }
    FFRooFitTree(TTree* tree, Int_t nVar,
                 const Char_t* name = "FFRooFitTree", const Char_t* title = "a FooFit RooFit",
                 const Char_t* weightVar = 0);
    virtual ~FFRooFitTree();

    TTree* GetTree() const { return fTree; }

    void SetTree(TTree* tree) { fTree = tree; }

    ClassDef(FFRooFitTree, 0)  // Fit trees using RooFit
};

#endif

