/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitHist                                                         //
//                                                                      //
// Class performing RooFit fits of histograms.                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooFitHist
#define FOOFIT_FFRooFitHist

#include "FFRooFit.h"

class TH1;

class FFRooFitHist : public FFRooFit
{

protected:
    TH1* fHist;                     // histogram (not owned)

    virtual Bool_t LoadData();

public:
    FFRooFitHist() : FFRooFit(),
                     fHist(0) { }
    FFRooFitHist(TH1* hist, Int_t nVar,
                 const Char_t* name = "FFRooFitHist", const Char_t* title = "a FooFit RooFit");
    virtual ~FFRooFitHist() { }

    TH1* GetHistogram() const { return fHist; }

    void SetHistogram(TH1* hist) { fHist = hist; }

    ClassDef(FFRooFitHist, 0)  // Fit histograms using RooFit
};

#endif

