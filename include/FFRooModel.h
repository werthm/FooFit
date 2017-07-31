/*************************************************************************
 * Author: Dominik Werthmueller, 2015
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
    RooAbsPdf* fPdf;                // model pdf
    Int_t fNPar;                    // number of parameters
    RooRealVar** fPar;              //[fNPar] parameter array
    Int_t fNVarTrans;               // number of variable transformations
    RooAbsReal** fVarTrans;         //[fNVarTrans] variable transformation array

    Bool_t CheckParBounds(Int_t par, const Char_t* loc) const;
    void AddParameter(Int_t i, const Char_t* name, const Char_t* title);
    void AddVarTrans(RooAbsReal* varTrans);

public:
    FFRooModel() : TNamed(),
                   fPdf(0),
                   fNPar(0), fPar(0),
                   fNVarTrans(0), fVarTrans(0) { }
    FFRooModel(const Char_t* name, const Char_t* title, Int_t nPar);
    virtual ~FFRooModel();

    RooAbsPdf* GetPdf() const { return fPdf; }
    Int_t GetNPar() const { return fNPar; }
    RooRealVar* GetPar(Int_t i) const;
    Double_t GetParameter(Int_t i) const;
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

    virtual void BuildModel(RooRealVar** vars) = 0;

    ClassDef(FFRooModel, 0)  // Abstract RooFit model class
};

#endif

