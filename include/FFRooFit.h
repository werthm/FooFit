/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFit                                                             //
//                                                                      //
// Abstract class performing RooFit fits.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef FOOFIT_FFRooFit
#define FOOFIT_FFRooFit

#include "TObject.h"

class RooRealVar;
class RooAbsData;
class RooAbsPdf;
class RooPlot;
class RooFitResult;
class FFRooModel;
class TCanvas;
class TH2;

class FFRooFit : public TObject
{

protected:
    Int_t fNVar;                    // number of fit variables
    RooRealVar** fVar;              //[fNVar] array of fit variables
    Int_t fNVarAux;                 // number of auxiliary variables (not to be fitted)
    RooRealVar** fVarAux;           //[fNVarAux] array of auxiliary variables (elements not owned)
    RooAbsData* fData;              // dataset
    FFRooModel* fModel;             // model (not owned)
    RooFitResult* fResult;          // result of last fit

    Bool_t CheckVarBounds(Int_t var, const Char_t* loc) const;
    Bool_t CheckVariables() const;
    Bool_t ContainsVariable(RooAbsPdf* pdf, Int_t var, Bool_t excl = kFALSE) const;
    virtual Bool_t LoadData() = 0;
    virtual Bool_t PreFit() = 0;
    virtual Bool_t PostFit() = 0;

    static const Color_t fgColors[8];     // some colors

public:
    FFRooFit() : TObject(),
                 fNVar(0), fVar(0),
                 fNVarAux(0), fVarAux(0),
                 fData(0), fModel(0),
                 fResult(0) { }
    FFRooFit(Int_t nVar);
    virtual ~FFRooFit();

    Int_t GetNVariable() const { return fNVar; }
    RooRealVar** GetVariables() const { return fVar; }
    RooRealVar* GetVariable(Int_t i) const;
    RooAbsData* GetData() const { return fData; }
    FFRooModel* GetModel() const { return fModel; }
    RooFitResult* GetResult() const { return fResult; }

    void SetVariable(Int_t i, const Char_t* name, const Char_t* title,
                     Double_t min, Double_t max, Int_t nbins = 0);
    void SetModel(FFRooModel* model) { fModel = model; }
    void AddAuxVariable(RooRealVar* aux_var);

    virtual Bool_t Fit();
    TCanvas* DrawFit();
    RooPlot* PlotDataAndModel(Int_t var);
    TH2* PlotData2D(Int_t var0, Int_t var1);
    TH2* PlotModel2D(Int_t var0, Int_t var1);

    ClassDef(FFRooFit, 0)  // Abstract RooFit fit class
};

#endif

