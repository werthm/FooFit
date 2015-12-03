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
class RooDataHist;

class FFRooModelHist : public FFRooModel
{

protected:
    TH1* fHist;                     // ROOT data histogram (not owned)
    RooDataHist* fDataHist;         // data histogram

public:
    FFRooModelHist() : FFRooModel(),
                       fHist(0), fDataHist(0) { }
    FFRooModelHist(const Char_t* name, const Char_t* title, TH1* hist);
    virtual ~FFRooModelHist();

    virtual void BuildModel(RooRealVar** vars);

    ClassDef(FFRooModelHist, 0)  // RooFit histogram model class
};

#endif

