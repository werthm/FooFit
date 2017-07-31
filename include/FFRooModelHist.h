/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelHist                                                       //
//                                                                      //
// Class representing a model from a histogram for RooFit.              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelHist
#define FOOFIT_FFRooModelHist

#include "FFRooModel.h"

class TH1;
class TTree;
class RooDataHist;

class FFRooModelHist : public FFRooModel
{

protected:
    Int_t fNDim;                    // number of dimensions
    TH1* fHist;                     // binned input data
    TTree* fTree;                   // unbinned input data
    Char_t* fWeightVar;             // event weight variable for unbin. input data
    RooDataHist* fDataHist;         // data histogram

public:
    FFRooModelHist() : FFRooModel(),
                       fNDim(0),
                       fHist(0), fTree(0),
                       fWeightVar(0),
                       fDataHist(0) { }
    FFRooModelHist(const Char_t* name, const Char_t* title, TH1* hist,
                   Bool_t addShiftPar = kFALSE);
    FFRooModelHist(const Char_t* name, const Char_t* title, Int_t Dim, TTree* tree,
                   const Char_t* weightVar = 0, Bool_t addShiftPar = kFALSE);
    virtual ~FFRooModelHist();

    virtual void BuildModel(RooRealVar** vars);

    ClassDef(FFRooModelHist, 0)  // RooFit histogram model class
};

#endif

