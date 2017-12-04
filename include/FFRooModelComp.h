/*************************************************************************
 * Author: Dominik Werthmueller, 2017
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelComp                                                       //
//                                                                      //
// Abstract class representing a composite model for RooFit.            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelComp
#define FOOFIT_FFRooModelComp

#include "FFRooModel.h"

class FFRooModelComp : public FFRooModel
{

protected:
    Int_t fNModel;                          // number of sub-models
    FFRooModel** fModelList;                //[fNModel] list of sub-models (elements not owned)

    Bool_t CheckModelBounds(Int_t mod, const Char_t* loc) const;

public:
    FFRooModelComp() : FFRooModel(),
                       fNModel(0), fModelList(0) { }
    FFRooModelComp(const Char_t* name, const Char_t* title, Int_t nPar,
                   Int_t n, FFRooModel** list = 0);
    virtual ~FFRooModelComp();

    Int_t GetNModel() const { return fNModel; }
    FFRooModel* GetModel(Int_t i) const { return fModelList[i]; }

    void SetModelList(FFRooModel** list);
    void SetModel(Int_t i, FFRooModel* model);

    virtual void BuildModel(RooRealVar** vars) = 0;

    ClassDef(FFRooModelComp, 0)  // RooFit composite model class
};

#endif

