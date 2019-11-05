/*************************************************************************
 * Author: Dominik Werthmueller, 2017-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitterSPlot                                                     //
//                                                                      //
// Class for fitting multiple species to data in a tree and deriving    //
// event weights using sPlot.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooFitterSPlot
#define FOOFIT_FFRooFitterSPlot

#include "FFRooFitterUnbinned.h"

class FFRooFitterSPlot : public FFRooFitterUnbinned
{

public:
    FFRooFitterSPlot() : FFRooFitterUnbinned() { }
    FFRooFitterSPlot(TTree* tree,
                     Int_t nVar, Int_t nSpec,
                     const Char_t* name, const Char_t* title,
                     const Char_t* evIDVar = "event_id", const Char_t* weightVar = 0);
    FFRooFitterSPlot(const Char_t* treeName, const Char_t* treeLoc,
                     Int_t nVar, Int_t nSpec,
                     const Char_t* name, const Char_t* title,
                     const Char_t* evIDVar = "event_id", const Char_t* weightVar = 0);
    virtual ~FFRooFitterSPlot() { }

    Double_t GetSpeciesWeight(Int_t event, Int_t i) const;
    TTree* GetSpeciesWeightsTree(const Char_t* name = 0,
                                 Int_t nSpec = 0, Int_t* spec = 0) const;

    ClassDef(FFRooFitterSPlot, 0)  // Class for species fitting/weighting using tree data
};

#endif

