/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitHist                                                         //
//                                                                      //
// Class performing RooFit fits of histograms.                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooGlobalFunc.h"
#include "TH1.h"

#include "FFRooFitHist.h"

ClassImp(FFRooFitHist)

//______________________________________________________________________________
FFRooFitHist::FFRooFitHist(TH1* hist, Int_t nVar,
                           const Char_t* name, const Char_t* title)
    : FFRooFit(nVar, name, title)
{
    // Constructor using the histogram 'hist' and 'nVar' fit variables.

    // init members
    fHist = hist;
}

//______________________________________________________________________________
Bool_t FFRooFitHist::LoadData()
{
    // Load the fit data.
    // Return kTRUE on success, otherwise kFALSE.

    // check fit variables
    if (!CheckVariables()) return kFALSE;

    // check histogram
    if (!fHist)
    {
        Error("LoadData", "Input data histogram was not set!");
        return kFALSE;
    }

    // create argument set of variables and auxiliary variables
    RooArgSet varSet;
    for (Int_t i = 0; i < fNVar; i++) varSet.add(*fVar[i]);
    for (Int_t i = 0; i < fNVarAux; i++) varSet.add(*fVarAux[i]);

    // create RooFit dataset
    if (fData) delete fData;
    fData = new RooDataHist(fHist->GetName(), fHist->GetTitle(), varSet, RooFit::Import(*fHist));

    return kTRUE;
}

