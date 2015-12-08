/*************************************************************************
 * Author: Dominik Werthmueller, 2015
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

class TChain;

class FFRooFitTree : public FFRooFit
{

protected:
    TChain* fChain;                 // chain of trees (not owned)

    virtual Bool_t LoadData();
    virtual Bool_t PreFit();
    virtual Bool_t PostFit();

public:
    FFRooFitTree() : FFRooFit(),
                     fChain(0) { }
    FFRooFitTree(TChain* chain, Int_t nVar,
                 const Char_t* name = "FFRooFitTree", const Char_t* title = "a FooFit RooFit");
    virtual ~FFRooFitTree() { }

    TChain* GetChain() const { return fChain; }

    void SetChain(TChain* chain) { fChain = chain; }

    ClassDef(FFRooFitTree, 0)  // Fit trees using RooFit
};

#endif

