/*************************************************************************
 * Author: Dominik Werthmueller, 2017-2019
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
    FFRooModel* fModel;             //! FooFit model
    TString fModelClass;            // class name of FooFit model
    Int_t fNModelPar;               // number of model parameters
    TString** fModelParName;        //[fNModelPar] array of model parameter names
    Double_t* fModelParValue;       //[fNModelPar] array of model parameter values
    Double_t* fModelParError;       //[fNModelPar] array of model parameter errors
    Double_t fYieldInit;            // initial value of yield parameter
    Double_t fYieldFit;             // fitted value of yield parameter
    Double_t fYieldFitError;        // error of fitted value of yield parameter
    Double_t fYieldMin;             // minimum value of yield parameter
    Double_t fYieldMax;             // maximum value of yield parameter
    Double_t fYieldConstrGMean;     // mean of Gaussian constraint on yield parameter
    Double_t fYieldConstrGSigma;    // sigma of Gaussian constraint on yield parameter

public:
    FFRooFitterSpecies() : TNamed(),
                           fModel(0), fModelClass(""),
                           fNModelPar(0), fModelParName(0), fModelParValue(0), fModelParError(0),
                           fYieldInit(0),
                           fYieldFit(0), fYieldFitError(0),
                           fYieldMin(0), fYieldMax(0),
                           fYieldConstrGMean(0), fYieldConstrGSigma(0) { }
    FFRooFitterSpecies(const Char_t* name, const Char_t* title);
    FFRooFitterSpecies(const Char_t* name, const Char_t* title,
                       FFRooModel* model);
    virtual ~FFRooFitterSpecies();

    FFRooModel* GetModel() const { return fModel; }
    const TString& GetModelClass() const { return fModelClass; }
    Int_t GetNModelPar() const { return fNModelPar; }
    const TString* GetModelParName(Int_t p) const { return fModelParName[p]; }
    Double_t GetModelParValue(Int_t p) const { return fModelParValue[p]; }
    Double_t GetModelParError(Int_t p) const { return fModelParError[p]; }
    Double_t GetYieldInit() const { return fYieldInit; }
    Double_t GetYieldFit() const { return fYieldFit; }
    Double_t GetYieldFitError() const { return fYieldFitError; }
    Double_t GetYieldMin() const { return fYieldMin; }
    Double_t GetYieldMax() const { return fYieldMax; }
    Double_t GetYieldConstrGMean() const { return fYieldConstrGMean; }
    Double_t GetYieldConstrGSigma() const { return fYieldConstrGSigma; }

    void SetYield(Double_t init, Double_t min, Double_t max);
    void SetYieldFit(Double_t y) { fYieldFit = y; }
    void SetYieldFitError(Double_t e) { fYieldFitError = e; }
    void SetYieldConstrGauss(Double_t mean, Double_t sigma)
    {
        fYieldConstrGMean = mean;
        fYieldConstrGSigma = sigma;
    }

    void UpdateModelParameters();

    virtual void Print(Option_t* option = "") const;

    ClassDef(FFRooFitterSpecies, 1)  // Species to be fit to data
};

#endif

