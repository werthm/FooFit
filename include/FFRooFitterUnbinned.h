/*************************************************************************
 * Author: Dominik Werthmueller, 2017-2018
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitterUnbinned                                                  //
//                                                                      //
// Class for fitting multiple species to unbinned data.                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooFitterUnbinned
#define FOOFIT_FFRooFitterUnbinned

#include "FFRooFitter.h"

class TTree;
class TH1;

class FFRooFitterUnbinned : public FFRooFitter
{

protected:
    TTree* fTree;                   // input tree
    Char_t* fWeightVar;             // name of event weight variable

public:
    FFRooFitterUnbinned(): FFRooFitter(),
                           fTree(0),
                           fWeightVar(0) { }
    FFRooFitterUnbinned(TTree* tree, Int_t nVar,
                        const Char_t* name, const Char_t* title,
                        const Char_t* weightVar = 0);
    FFRooFitterUnbinned(const Char_t* treeName, const Char_t* treeLoc, Int_t nVar,
                        const Char_t* name, const Char_t* title,
                        const Char_t* weightVar = 0);
    virtual ~FFRooFitterUnbinned();

    void SetVariableAutoRange(Int_t i, const Char_t* name, const Char_t* title,
                              Int_t nbins);

    Bool_t AddSpeciesHistPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                             Bool_t addShiftPar = kFALSE, Int_t intOrder = 0);
    Bool_t AddSpeciesHistPdf(const Char_t* name, const Char_t* title, TH1* hist,
                             Bool_t addShiftPar = kFALSE, Int_t intOrder = 0);
    Bool_t AddSpeciesKeysPdf(const Char_t* name, const Char_t* title, const Char_t* treeLoc,
                             const Char_t* opt = "a", Double_t rho = 1, Int_t nSigma = 3, Bool_t rotate = kTRUE);

    ClassDef(FFRooFitterUnbinned, 0)  // Class for species fitting to unbinned data
};

#endif

