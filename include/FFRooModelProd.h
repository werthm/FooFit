/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2017
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelProd                                                       //
//                                                                      //
// Class representing a product of models for RooFit.                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelProd
#define FOOFIT_FFRooModelProd

#include "FFRooModelComp.h"

class FFRooModelProd : public FFRooModelComp
{

public:
    FFRooModelProd() : FFRooModelComp() { }
    FFRooModelProd(const Char_t* name, const Char_t* title, Int_t n, FFRooModel** list = 0);
    virtual ~FFRooModelProd() { }

    virtual void BuildModel(RooRealVar** vars);

    ClassDef(FFRooModelProd, 0)  // RooFit product of models class
};

#endif

