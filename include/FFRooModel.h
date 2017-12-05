/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2017
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModel                                                           //
//                                                                      //
// Abstract model class for RooFit.                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooModel
#define FOOFIT_FFRooModel

#include "TNamed.h"

class RooAbsPdf;
class RooRealVar;
class RooAbsReal;

class FFRooModel : public TNamed
{

protected:
    RooAbsPdf* fPdf;                //! model pdf
    Int_t fNPar;                    // number of parameters
    RooRealVar** fPar;              //[fNPar] parameter array
    Int_t fNVarTrans;               // number of variable transformations
    RooAbsReal** fVarTrans;         //[fNVarTrans] variable transformation array
    Int_t fNConstr;                 // number of constraints
    FFRooModel** fConstr;           //[fNConstr] array of constraints

    Bool_t CheckParBounds(Int_t par, const Char_t* loc) const;
    void AddParameter(Int_t i, const Char_t* name, const Char_t* title);
    void AddVarTrans(RooAbsReal* varTrans);
    void AddConstraint(FFRooModel* c);

public:
    FFRooModel() : TNamed(),
                   fPdf(0),
                   fNPar(0), fPar(0),
                   fNVarTrans(0), fVarTrans(0),
                   fNConstr(0), fConstr(0) { }
    FFRooModel(const Char_t* name, const Char_t* title, Int_t nPar);
    virtual ~FFRooModel();

    RooAbsPdf* GetPdf() const { return fPdf; }
    Int_t GetNPar() const { return fNPar; }
    const RooRealVar* GetPar(Int_t i) const;
    Double_t GetParameter(Int_t i) const;
    Double_t GetParError(Int_t i) const;
    Bool_t IsParConstant(Int_t i) const;
    const Char_t* GetParName(Int_t i) const;
    const Char_t* GetParTitle(Int_t i) const;
    Int_t GetNVarTrans() const { return fNVarTrans; }
    RooAbsReal* GetVarTrans(Int_t i) const { return fVarTrans[i]; }

    void SetParameter(Int_t i, Double_t v);
    void SetParameter(Int_t i, Double_t v, Double_t min, Double_t max);
    void SetParLimits(Int_t i, Double_t min, Double_t max);
    void SetParName(Int_t i, const Char_t* name);
    void SetParTitle(Int_t i, const Char_t* title);
    void FixParameter(Int_t i, Double_t v);

    void AddParConstrGauss(Int_t i, Double_t mean, Double_t sigma);
    void FindAllConstraints(TList* list);

    virtual void BuildModel(RooRealVar** vars) = 0;

    virtual void Print(Option_t* option = "") const;

    ClassDef(FFRooModel, 1)  // Abstract RooFit model class
};

#endif

