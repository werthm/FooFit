/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelExpo                                                       //
//                                                                      //
// 1-dim. exponential model for RooFit.                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelExpo
#define FOOFIT_FFRooModelExpo

#include "FFRooModel.h"

class FFRooModelExpo : public FFRooModel
{

public:
    FFRooModelExpo() : FFRooModel() { }
    FFRooModelExpo(const Char_t* name, const Char_t* title);
    virtual ~FFRooModelExpo() { }

    virtual void BuildModel(RooRealVar** vars);

    ClassDef(FFRooModelExpo, 0)  // 1-dim. exponential RooFit model
};

#endif

