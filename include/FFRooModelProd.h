/*************************************************************************
 * Author: Dominik Werthmueller, 2015
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

#include "FFRooModel.h"

class FFRooModelProd : public FFRooModel
{

protected:
    Int_t fNModel;                          // number of models to multiply
    FFRooModel** fModelList;                //[fNModel] list of models to multiply (elements not owned)

public:
    FFRooModelProd() : FFRooModel(),
                       fNModel(0), fModelList(0) { }
    FFRooModelProd(const Char_t* name, const Char_t* title, Int_t n, FFRooModel** list = 0);
    virtual ~FFRooModelProd();

    void SetModelList(FFRooModel** list);

    virtual void BuildModel(RooRealVar** vars);

    ClassDef(FFRooModelProd, 0)  // RooFit product of models class
};

#endif

