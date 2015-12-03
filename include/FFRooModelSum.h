/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelSum                                                        //
//                                                                      //
// Class representing a sum of models for RooFit.                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModelSum
#define FOOFIT_FFRooModelSum

#include "FFRooModel.h"

class FFRooModelSum : public FFRooModel
{

protected:
    FFRooModel** fModelList;                //[fNPar] list of models to sum (elements not owned)

public:
    FFRooModelSum() : FFRooModel(),
                      fModelList(0) { }
    FFRooModelSum(const Char_t* name, const Char_t* title, Int_t n, FFRooModel** list = 0);
    virtual ~FFRooModelSum();

    void SetModelList(FFRooModel** list);

    virtual void BuildModel(RooRealVar** vars);

    ClassDef(FFRooModelSum, 0)  // RooFit sum of models class
};

#endif

