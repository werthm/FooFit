/*************************************************************************
 * Author: Dominik Werthmueller, 2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelLandau                                                     //
//                                                                      //
// Landau model for RooFit.                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelLandau
#define FOOFIT_FFRooModelLandau

#include "FFRooModel.h"

class FFRooModelLandau : public FFRooModel
{

public:
    FFRooModelLandau() : FFRooModel() { }
    FFRooModelLandau(const Char_t* name, const Char_t* title,
                     Bool_t gaussConvol = kFALSE);
    virtual ~FFRooModelLandau() { }

    virtual void BuildModel(RooAbsReal** vars);

    ClassDef(FFRooModelLandau, 0)  // Landau RooFit model
};

#endif

