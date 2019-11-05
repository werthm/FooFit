/*************************************************************************
 * Author: Dominik Werthmueller, 2017-2019
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

class FFRooFitterUnbinned : public FFRooFitter
{

public:
    FFRooFitterUnbinned(): FFRooFitter() { }
    FFRooFitterUnbinned(TTree* tree, Int_t nVar,
                        const Char_t* name, const Char_t* title,
                        const Char_t* weightVar = 0);
    FFRooFitterUnbinned(const Char_t* treeName, const Char_t* treeLoc, Int_t nVar,
                        const Char_t* name, const Char_t* title,
                        const Char_t* weightVar = 0);
    virtual ~FFRooFitterUnbinned() { }

    ClassDef(FFRooFitterUnbinned, 0)  // Class for species fitting to unbinned data
};

#endif

