/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelPol                                                        //
//                                                                      //
// 1-dim. polynomial model for RooFit.                                  //
// Note: Parameters start at order 0.                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelPol
#define FOOFIT_FFRooModelPol

#include "FFRooModel.h"

class FFRooModelPol : public FFRooModel
{

public:
    FFRooModelPol() : FFRooModel() { }
    FFRooModelPol(const Char_t* name, const Char_t* title, Int_t nOrder);
    virtual ~FFRooModelPol() { }

    Int_t GetNOrder() const { return fNPar-1; }

    virtual void BuildModel(RooAbsReal** vars);

    ClassDef(FFRooModelPol, 0)  // 1-dim. polynomial RooFit model
};

#endif

