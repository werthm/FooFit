/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2019
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

#include "FFRooModelComp.h"

class FFRooModelSum : public FFRooModelComp
{

public:
    FFRooModelSum() : FFRooModelComp() { }
    FFRooModelSum(const Char_t* name, const Char_t* title, Int_t n, FFRooModel** list = 0);
    virtual ~FFRooModelSum() { }

    virtual void BuildModel(RooAbsReal** vars);

    virtual void Print(Option_t* option = "") const;

    ClassDef(FFRooModelSum, 0)  // RooFit sum of models class
};

#endif

