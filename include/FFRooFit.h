/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2016
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

#include "TNamed.h"
#include "RooCmdArg.h"

class RooRealVar;
class RooAbsData;
class RooAbsPdf;
class RooPlot;
class RooFitResult;
class FFRooModel;
class TCanvas;
class TH2;

class FFRooFit : public TNamed
{

public:
    // Minimizer types
    enum EFFMinimizer {
        kMinuit,
        kMinuit2_Migrad
    };
    typedef EFFMinimizer FFMinimizer_t;

protected:
    Int_t fNVar;                    // number of fit variables
    RooRealVar** fVar;              //[fNVar] array of fit variables
    Int_t fNVarAux;                 // number of auxiliary variables (not to be fitted)
    RooRealVar** fVarAux;           //[fNVarAux] array of auxiliary variables (elements not owned)
    Int_t fNVarCtrl;                // number of control variables
    RooRealVar** fVarCtrl;          //[fNCtrlVar] array of control variables (subset of aux. vars.)
    RooAbsData* fData;              // dataset
    FFRooModel* fModel;             // model (not owned)
    RooFitResult* fResult;          // result of last fit
    Int_t fNChi2PreFit;             // number of chi2 pre-fits
    FFMinimizer_t fMinimizer;       // type of minimizer

    Bool_t CheckVarBounds(Int_t var, const Char_t* loc) const;
    Bool_t CheckVariables() const;
    Bool_t ContainsVariable(RooAbsPdf* pdf, Int_t var, Bool_t excl = kFALSE) const;
    RooCmdArg CreateMinimizerArg(FFMinimizer_t min);
    virtual Bool_t LoadData() = 0;
    virtual Bool_t PrepareFit();
    virtual Bool_t PostFit();
    Bool_t Chi2PreFit();

    static const Color_t fgColors[8];    // some colors
    static const Style_t fgLStyle[3];    // line styles

public:
    FFRooFit() : TNamed(),
                 fNVar(0), fVar(0),
                 fNVarAux(0), fVarAux(0),
                 fNVarCtrl(0), fVarCtrl(0),
                 fData(0), fModel(0),
                 fResult(0),
                 fNChi2PreFit(0),
                 fMinimizer(kMinuit2_Migrad) { }
    FFRooFit(Int_t nVar, const Char_t* name = "FFRooFit", const Char_t* title = "a FooFit RooFit");
    virtual ~FFRooFit();

    Int_t GetNVariable() const { return fNVar; }
    RooRealVar** GetVariables() const { return fVar; }
    RooRealVar* GetVariable(Int_t i) const;
    RooAbsData* GetData() const { return fData; }
    FFRooModel* GetModel() const { return fModel; }
    RooFitResult* GetResult() const { return fResult; }
    Int_t GetNChi2PreFit() const { return fNChi2PreFit; }
    FFMinimizer_t GetMinimizer() const { return fMinimizer; }

    void SetVariable(Int_t i, const Char_t* name, const Char_t* title,
                     Double_t min, Double_t max, Int_t nbins = 0);
    void SetModel(FFRooModel* model) { fModel = model; }
    void AddAuxVariable(RooRealVar* aux_var);
    void AddControlVariable(const Char_t* name, const Char_t* title);
    void SetNChi2PreFit(Int_t n) { fNChi2PreFit = n; }
    void SetMinimizer(FFMinimizer_t min) { fMinimizer = min; }

    virtual Bool_t Fit();
    TCanvas* DrawFit();
    RooPlot* PlotDataAndModel(Int_t var);
    TH2* PlotData2D(Int_t var0, Int_t var1);
    TH2* PlotModel2D(Int_t var0, Int_t var1);

    ClassDef(FFRooFit, 0)  // Abstract RooFit fit class
};

#endif

