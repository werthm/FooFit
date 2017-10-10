/*************************************************************************
 * Author: Dominik Werthmueller, 2017
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitterTree                                                      //
//                                                                      //
// Class for fitting multiple species to data in a tree.                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooFitterTree
#define FOOFIT_FFRooFitterTree

#include "FFRooFitter.h"

class TTree;

class FFRooFitterTree : public FFRooFitter
{

protected:
    TTree* fTree;                   // input tree
    Char_t* fWeightVar;             // name of event weight variable

    virtual Bool_t PrepareFit();

public:
    FFRooFitterTree(): FFRooFitter(),
                       fTree(0),
                       fWeightVar(0) { }
    FFRooFitterTree(TTree* tree, Int_t nVar,
                    const Char_t* name, const Char_t* title,
                    const Char_t* weightVar = 0);
    FFRooFitterTree(const Char_t* treeName, const Char_t* treeLoc, Int_t nVar,
                    const Char_t* name, const Char_t* title,
                    const Char_t* weightVar = 0);
    virtual ~FFRooFitterTree();

    void SetVariableAutoRange(Int_t i, const Char_t* name, const Char_t* title,
                              Int_t nbins);

    Bool_t AddSpeciesHistPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                             Bool_t addShiftPar = kFALSE);
    Bool_t AddSpeciesHistPdf(const Char_t* name, const Char_t* title, TH1* hist,
                             Bool_t addShiftPar = kFALSE);
    Bool_t AddSpeciesKeysPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                             const Char_t* opt = "a", Double_t rho = 1, Int_t nSigma = 3, Bool_t rotate = kTRUE);

    ClassDef(FFRooFitterTree, 0)  // Class for species fitting to tree data
};

#endif

