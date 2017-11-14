/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelKeys                                                       //
//                                                                      //
// Class representing a model for RooFit using the n-dimensional kernel //
// estimation pdf of RooNDKeysPdf.                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelKeys
#define FOOFIT_FFRooModelKeys

#include "FFRooModel.h"

class TTree;
class RooDataSet;

class FFRooModelKeys : public FFRooModel
{

protected:
    Int_t fNDim;                    // number of dimensions
    TTree* fTree;                   // event tree
    RooDataSet* fDataSet;           // RooFit dataset
    TString fOpt;                   // option for RooNDKeysPdf
    Double_t fRho;                  // rho parameter for RooNDKeysPdf
    Int_t fNSigma;                  // nSigma parameter for RooNDKeysPdf
    Bool_t fRotate;                 // rotate parameter for RooNDKeysPdf

public:
    FFRooModelKeys() : FFRooModel(),
                       fNDim(0),
                       fTree(0), fDataSet(0),
                       fOpt(""), fRho(0), fNSigma(0), fRotate(kTRUE) { }
    FFRooModelKeys(const Char_t* name, const Char_t* title, Int_t nDim, TTree* tree,
                   const Char_t* opt = "a", Double_t rho = 1, Int_t nSigma = 3, Bool_t rotate = kTRUE);
    virtual ~FFRooModelKeys();

    virtual void BuildModel(RooRealVar** vars);

    ClassDef(FFRooModelKeys, 0)  // RooFit kernel estimation model class
};

#endif

