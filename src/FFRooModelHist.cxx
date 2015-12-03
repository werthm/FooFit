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


#include "TH1.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"

#include "FFRooModelHist.h"

ClassImp(FFRooModelHist)

//______________________________________________________________________________
FFRooModelHist::FFRooModelHist(const Char_t* name, const Char_t* title, TH1* hist)
    : FFRooModel(name, title, 0)
{
    // Constructor.

    // init members
    fHist = hist;
    fDataHist = 0;
}

//______________________________________________________________________________
FFRooModelHist::~FFRooModelHist()
{
    // Destructor.

    if (fDataHist) delete fDataHist;
}

//______________________________________________________________________________
void FFRooModelHist::BuildModel(RooRealVar** vars)
{
    // Build the model using the variables 'vars'.

    // prepare variable set based on histogram dimension
    RooArgSet varSet;
    for (Int_t i = 0; i < fHist->GetDimension(); i++)
        varSet.add(*vars[i]);

    // create RooFit histogram
    if (fDataHist) delete fDataHist;
    fDataHist = new RooDataHist(TString::Format("%s_RooFit", fHist->GetName()),
                                TString::Format("%s (RooFit)", fHist->GetTitle()),
                                varSet, RooFit::Import(*fHist));

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooHistPdf(GetName(), GetTitle(), varSet, *fDataHist);
}

