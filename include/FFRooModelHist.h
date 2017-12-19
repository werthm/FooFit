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
    TString fWeightVar;             // event weight variable for unbin. input data
    RooDataHist* fDataHist;         // data histogram

    void DetermineHistoBinning(RooRealVar* var, RooRealVar* par,
                               Int_t* nBin, Double_t* min, Double_t* max);

public:
    FFRooModelHist() : FFRooModel(),
                       fNDim(0),
                       fHist(0), fTree(0),
                       fWeightVar(""),
                       fDataHist(0) { }
    FFRooModelHist(const Char_t* name, const Char_t* title, TH1* hist,
                   Bool_t addShiftPar = kFALSE);
    FFRooModelHist(const Char_t* name, const Char_t* title, Int_t Dim, TTree* tree,
                   const Char_t* weightVar = 0, Bool_t addShiftPar = kFALSE);
    virtual ~FFRooModelHist();

    TH1* GetHistogram() const { return fHist; }
    RooDataHist* GetDataHistogram() const { return fDataHist; }

    virtual void BuildModel(RooRealVar** vars);

    ClassDef(FFRooModelHist, 0)  // RooFit histogram model class
};

#endif

