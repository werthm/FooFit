/*************************************************************************
 * Author: Dominik Werthmueller, 2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelGaussBifur                                                 //
//                                                                      //
// 1-dim. bifurcated Gaussian model for RooFit.                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelGaussBifur
#define FOOFIT_FFRooModelGaussBifur

#include "FFRooModel.h"

class FFRooModelGaussBifur : public FFRooModel
{

public:
    FFRooModelGaussBifur() : FFRooModel() { }
    FFRooModelGaussBifur(const Char_t* name, const Char_t* title);
    virtual ~FFRooModelGaussBifur() { }

    virtual void BuildModel(RooAbsReal** vars);

    ClassDef(FFRooModelGaussBifur, 0)  // 1-dim. bifurcated Gaussian RooFit model
};

#endif

