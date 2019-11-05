/*************************************************************************
 * Author: Dominik Werthmueller, 2018-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelChebychev                                                  //
//                                                                      //
// 1-dim. Chebychev polynomial model for RooFit.                        //
// Note: Parameters start at order 1.                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelChebychev
#define FOOFIT_FFRooModelChebychev

#include "FFRooModel.h"

class FFRooModelChebychev : public FFRooModel
{

public:
    FFRooModelChebychev() : FFRooModel() { }
    FFRooModelChebychev(const Char_t* name, const Char_t* title, Int_t nOrder);
    virtual ~FFRooModelChebychev() { }

    Int_t GetNOrder() const { return fNPar; }

    virtual void BuildModel(RooAbsReal** vars);

    ClassDef(FFRooModelChebychev, 0)  // 1-dim. Chebychev polynomial RooFit model
};

#endif

