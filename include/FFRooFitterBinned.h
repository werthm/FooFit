/*************************************************************************
 * Author: Dominik Werthmueller, 2018-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitterBinned                                                    //
//                                                                      //
// Class for fitting multiple species to binned data.                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooFitterBinned
#define FOOFIT_FFRooFitterBinned

#include "FFRooFitter.h"

class FFRooFitterBinned : public FFRooFitter
{

public:
    FFRooFitterBinned() : FFRooFitter() { }
    FFRooFitterBinned(TTree* tree, Int_t nVar,
                      const Char_t* name, const Char_t* title,
                      const Char_t* weightVar = 0);
    FFRooFitterBinned(const Char_t* treeName, const Char_t* treeLoc, Int_t nVar,
                      const Char_t* name, const Char_t* title,
                      const Char_t* weightVar = 0);
    virtual ~FFRooFitterBinned() { }

    ClassDef(FFRooFitterBinned, 0)  // Class for species fitting to binned data
};

#endif

