/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelGauss                                                      //
//                                                                      //
// 1-dim. Gaussian model for RooFit.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelGauss
#define FOOFIT_FFRooModelGauss

#include "FFRooModel.h"

class FFRooModelGauss : public FFRooModel
{

public:
    FFRooModelGauss() : FFRooModel() { }
    FFRooModelGauss(const Char_t* name, const Char_t* title);
    virtual ~FFRooModelGauss() { }

    virtual void BuildModel(RooAbsReal** vars);

    ClassDef(FFRooModelGauss, 0)  // 1-dim. Gaussian RooFit model
};

#endif

