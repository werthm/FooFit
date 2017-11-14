/*************************************************************************
 * Author: Dominik Werthmueller, 2017
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitterSpecies                                                   //
//                                                                      //
// Base class representing a species to be fit to data.                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooFitterSpecies
#define FOOFIT_FFRooFitterSpecies

#include "TNamed.h"

class FFRooModel;

class FFRooFitterSpecies : public TNamed
{

protected:
    FFRooModel* fModel;             // RooFit model
    Double_t fYieldInit;            // initial value of yield parameter
    Double_t fYieldFit;             // fitted value of yield parameter
    Double_t fYieldFitError;        // error of fitted value of yield parameter
    Double_t fYieldMin;             // minimum value of yield parameter
    Double_t fYieldMax;             // maximum value of yield parameter

public:
    FFRooFitterSpecies() : TNamed(),
                           fModel(0),
                           fYieldInit(0),
                           fYieldFit(0), fYieldFitError(0),
                           fYieldMin(0), fYieldMax(0) { }
    FFRooFitterSpecies(const Char_t* name, const Char_t* title);
    FFRooFitterSpecies(const Char_t* name, const Char_t* title,
                       FFRooModel* model);
    virtual ~FFRooFitterSpecies();

    FFRooModel* GetModel() const { return fModel; }
    Double_t GetYieldInit() const { return fYieldInit; }
    Double_t GetYieldFit() const { return fYieldFit; }
    Double_t GetYieldFitError() const { return fYieldFitError; }
    Double_t GetYieldMin() const { return fYieldMin; }
    Double_t GetYieldMax() const { return fYieldMax; }

    void SetYield(Double_t init, Double_t min, Double_t max);
    void SetYieldFit(Double_t y) { fYieldFit = y; }
    void SetYieldFitError(Double_t e) { fYieldFitError = e; }

    virtual void Print(Option_t* option = "") const;

    ClassDef(FFRooFitterSpecies, 1)  // Species to be fit to data
};

#endif

