/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2018
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFit                                                             //
//                                                                      //
// Abstract class performing RooFit fits.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooRealVar.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooPlot.h"
#include "RooDataHist.h"
#include "RooFitResult.h"
#include "RooChi2Var.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH2.h"
#include "TMath.h"

#include "FFRooFit.h"
#include "FFFooFit.h"
#include "FFRooModel.h"

ClassImp(FFRooFit)

// init static class members
const Color_t FFRooFit::fgColors[8] = { 4, 2, 6, 7, 9, 12, 28, 41 };
const Style_t FFRooFit::fgLStyle[3] = { kSolid, kDashed, kDotted };

//______________________________________________________________________________
FFRooFit::FFRooFit(Int_t nVar, const Char_t* name, const Char_t* title)
    : TNamed(name, title)
{
    // Constructor using 'nVar' fit variables.

    // init members
    fNVar = nVar;
    fVar = new RooRealVar*[fNVar];
    for (Int_t i = 0; i < fNVar; i++) fVar[i] = 0;
    fNVarAux = 0;
    fVarAux = 0;
    fNVarCtrl = 0;
    fVarCtrl = 0;
    fNConstr = 0;
    fConstr = 0;
    fData = 0;
    fModel = 0;
    fResult = 0;
    fNChi2PreFit = 0;
    fMinimizer = kMinuit2_Migrad;
    fMinimizerPreFit = kMinuit2_Migrad;
    fRangeMin = 0;
    fRangeMax = 0;
}

//______________________________________________________________________________
FFRooFit::~FFRooFit()
{
    // Destructor.

    if (fVar)
    {
        for (Int_t i = 0; i < fNVar; i++)
            if (fVar[i]) delete fVar[i];
        delete [] fVar;
    }
    if (fVarAux) delete [] fVarAux;
    if (fVarCtrl)
    {
        for (Int_t i = 0; i < fNVarCtrl; i++)
            if (fVarCtrl[i]) delete fVarCtrl[i];
        delete [] fVarCtrl;
    }
    if (fConstr) delete [] fConstr;
    if (fData) delete fData;
    if (fResult) delete fResult;
}

//______________________________________________________________________________
RooRealVar* FFRooFit::GetVariable(Int_t i) const
{
    // Return the fit variable with index 'i'.

    // check variable index
    if (CheckVarBounds(i, "GetVariable()")) return fVar[i];
    else return 0;
}

//______________________________________________________________________________
void FFRooFit::SetVariable(Int_t i, const Char_t* name, const Char_t* title,
                           Double_t min, Double_t max, Int_t nbins)
{
    // Set the fit variable with index 'i' to the name 'name' and title 'title', and
    // adjust the range to the interval [min,max]. Use 'nbins' bins when not zero.

    // check variable index
    if (CheckVarBounds(i, "SetVariable()"))
    {
        if (fVar[i]) delete fVar[i];
        fVar[i] = new RooRealVar(name, title, min, max);
        if (nbins) fVar[i]->setBins(nbins);
    }
}

//______________________________________________________________________________
void FFRooFit::AddAuxVariable(RooRealVar* aux_var)
{
    // Register 'aux_var' as auxiliary variable. This variable will not be fitted
    // but, depending on the FFRooFit child class, it will e.g. be loaded into the
    // dataset.

    // backup old array
    RooRealVar** old = fVarAux;

    // create new array
    fVarAux = new RooRealVar*[fNVarAux+1];
    for (Int_t i = 0; i < fNVarAux; i++) fVarAux[i] = old[i];

    // add new element
    fVarAux[fNVarAux] = aux_var;
    fNVarAux++;

    // destroy old list
    if (old) delete [] old;
}

//______________________________________________________________________________
void FFRooFit::AddControlVariable(const Char_t* name, const Char_t* title)
{
    // Add a control variable with name 'name' and title 'title' to the list
    // of control variables. The name of the variable has to match a column
    // in the RooDataSet.

    // backup old array
    RooRealVar** old = fVarCtrl;

    // create new array
    fVarCtrl = new RooRealVar*[fNVarCtrl+1];
    for (Int_t i = 0; i < fNVarCtrl; i++) fVarCtrl[i] = old[i];

    // add new element
    RooRealVar* v = new RooRealVar(name, title, -RooNumber::infinity(), RooNumber::infinity());
    fVarCtrl[fNVarCtrl] = v;
    fNVarCtrl++;

    // register control variable as auxiliary variable
    AddAuxVariable(v);

    // destroy old list
    if (old) delete [] old;
}

//______________________________________________________________________________
void FFRooFit::AddConstraint(FFRooModel* c)
{
    // Add the model pdf 'c' as fit constraint.

    // backup old array
    FFRooModel** old = fConstr;

    // create new array
    fConstr = new FFRooModel*[fNConstr+1];
    for (Int_t i = 0; i < fNConstr; i++) fConstr[i] = old[i];

    // add new element
    fConstr[fNConstr] = c;
    fNConstr++;

    // destroy old list
    if (old) delete [] old;
}

//______________________________________________________________________________
Bool_t FFRooFit::CheckVarBounds(Int_t var, const Char_t* loc) const
{
    // Check if the variable index 'var' is within valid bounds.
    // Return kTRUE if the index 'var' is valid, otherwise kFALSE.
    // Use 'loc' to set the location of the error.

    // check index range
    if (var < 0 || var >= fNVar)
    {
        Error("CheckVarBounds", "%s: Invalid variable index %d (number of variables: %d)",
              loc, var, fNVar);
        return kFALSE;
    }
    else return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFit::CheckVariables() const
{
    // Return kTRUE if all variables were set, otherwise kFALSE.

    // loop over all variables
    for (Int_t i = 0; i < fNVar; i++)
    {
        if (!fVar[i])
        {
            Error("CheckVariables", "Fit variable %d was not set!", i);
            return kFALSE;
        }
    }

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFit::CheckFitResult(RooFitResult* res, FFMinimizer_t minimizer,
                                Bool_t verbose) const
{
    // Check the fit result 'f' obtained using the minimizer 'minimizer' and
    // return kFALSE if the fit failed.
    // If 'verbose' is kFALSE, no messages will be printed.

    // check fit result: seems to be only working with Minuit2
    if (minimizer == kMinuit2_Migrad)
    {
        // loop over fit steps
        for (UInt_t i = 0; i < res->numStatusHistory(); i++)
        {
            // check status of fit step
            if (res->statusCodeHistory(i) != 0)
            {
                if (verbose) Error("CheckFitResult", "An error occurred during the fit routine (step %d (%s) returned status %d)!",
                                                     i, res->statusLabelHistory(i), res->statusCodeHistory(i));
                return kFALSE;
            }
        }
    }

    // check fit result by looking at the covariance matrix quality (only works for unweighted fits)
    // quality factor:
    // 0: Not calculated at all
    // 1: Diagonal approximation only, not accurate
    // 2: Full matrix, but forced positive-definite
    // 3: Full accurate covariance matrix (After MIGRAD, this is the indication of normal
    //     convergence.)
    if (!fData->isWeighted() && res->covQual() != 3)
    {
        if (verbose) Error("CheckFitResult", "Poor quality of covariance matrix (%d) - the fit probably failed!", res->covQual());
        return kFALSE;
    }

    // good fit here
    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFit::ContainsVariable(RooAbsPdf* pdf, Int_t var, Bool_t excl) const
{
    // Check if the pdf 'pdf' contains the variable with index 'var'.
    // If 'excl' is kTRUE, no other variable must be found in 'pdf'.

    // check exclusivity
    if (excl)
    {
        // loop over variables
        Bool_t corr = kFALSE;
        Bool_t other = kFALSE;
        for (Int_t i = 0; i < fNVar; i++)
        {
            // check variable
            if (pdf->getVariables()->find(fVar[i]->GetName()))
            {
                if (i == var) corr = kTRUE;
                else other = kTRUE;
            }
        }

        // check result
        if (corr && !other) return kTRUE;
        else return kFALSE;
    }
    else
    {
        // check only the corresponding variable
        if (pdf->getVariables()->find(fVar[var]->GetName())) return kTRUE;
        else return kFALSE;
    }
}

//______________________________________________________________________________
RooCmdArg FFRooFit::CreateMinimizerArg(FFMinimizer_t min)
{
    // Create the RooCmdArg for the minimizer type 'min'.

    // check type
    switch (min)
    {
        // Minuit
        case kMinuit_Migrad:
            return RooFit::Minimizer("Minuit", "Migrad");
        case kMinuit_Simplex:
            return RooFit::Minimizer("Minuit", "Simplex");
        case kMinuit_Minimize:
            return RooFit::Minimizer("Minuit", "Minimize");
        case kMinuit_Scan:
            return RooFit::Minimizer("Minuit", "Scan");
        case kMinuit_Seek:
            return RooFit::Minimizer("Minuit", "Seek");
        // Fumili
        case kFumili:
            return RooFit::Minimizer("Fumili");
        // Minuit2
        case kMinuit2_Migrad:
            return RooFit::Minimizer("Minuit2", "Migrad");
        case kMinuit2_Simplex:
            return RooFit::Minimizer("Minuit2", "Simplex");
        case kMinuit2_Minimize:
            return RooFit::Minimizer("Minuit2", "Minimize");
        case kMinuit2_Scan:
            return RooFit::Minimizer("Minuit2", "Scan");
        case kMinuit2_Combined:
            return RooFit::Minimizer("Minuit2", "Combined");
        case kMinuit2_Fumili2:
            return RooFit::Minimizer("Minuit2", "Fumili2");
        // GSL
        case kGSLMultiFit_LevenMarq:
            return RooFit::Minimizer("GSLMultiFit");
        case kGSLMultiMin_conjugatefr:
            return RooFit::Minimizer("GSLMultiMin", "conjugatefr");
        case kGSLMultiMin_conjugatepr:
            return RooFit::Minimizer("GSLMultiMin", "conjugatepr");
        case kGSLMultiMin_bfgs:
            return RooFit::Minimizer("GSLMultiMin", "bfgs");
        case kGSLMultiMin_bfgs2:
            return RooFit::Minimizer("GSLMultiMin", "bfgs2");
        case kGSLMultiMin_SteepestDescent:
            return RooFit::Minimizer("GSLMultiMin", "SteepestDescent");
        case kGSLSimAn:
            return RooFit::Minimizer("GSLSimAn");
        // Other
        case kGenetic:
            return RooFit::Minimizer("Genetic");
        default:
            return RooCmdArg::none();
    }
}

//______________________________________________________________________________
Bool_t FFRooFit::PrepareFit()
{
    // Perform tasks before fitting.
    // Return kTRUE on success, otherwise kFALSE.

    Char_t ws[256];
    Int_t maxLen;

    //
    // adjust range of fit variables
    //

    // loop over fit variables
    Double_t min, max;
    for (Int_t i = 0; i < fNVar; i++)
    {
        // get range of data set
        fData->getRange(*fVar[i], min, max);

        // round to next integer
        min = TMath::Floor(min);
        max = TMath::Ceil(max);

        // compare minimum values
        if (fVar[i]->getMin() < min)
        {
            min = TMath::Ceil(min);
            Warning("PrepareFit", "Adjusting minimum of variable '%s' to minimum of dataset: %f -> %f",
                                  fVar[i]->GetName(), fVar[i]->getMin(), min);
            fVar[i]->setMin(min);
        }

        // compare maximum values
        if (fVar[i]->getMax() > max)
        {
            max = TMath::Floor(max);
            Warning("PrepareFit", "Adjusting maximum of variable '%s' to maximum of dataset: %f -> %f",
                                  fVar[i]->GetName(), fVar[i]->getMax(), max);
            fVar[i]->setMax(max);
        }
    }

    //
    // calculate correlations between fit variables
    //

    if (fNVar > 1)
    {
        // formatting stuff
        maxLen = 0;
        for (Int_t i = 0; i < fNVar; i++)
            maxLen = TMath::Max(maxLen, (Int_t)strlen(fVar[i]->GetTitle()));

        printf("\n");
        printf("  Fit variable correlations\n");
        printf("\n");

        // loop over fit variables
        for (Int_t i = 0; i < fNVar; i++)
        {
            // loop over fit variables
            for (Int_t j = i+1; j < fNVar; j++)
            {
                if (j == i+1)
                    printf("    Fit variable '%s'\n", fVar[i]->GetTitle());

                // calculate correlation
                Double_t corr = fData->correlation(*fVar[i], *fVar[j]);

                // format and print
                Int_t l = 0;
                for (Int_t k = strlen(fVar[j]->GetTitle()); k <= maxLen; k++)
                {
                    ws[l] = ' ';
                    ws[l+1] = '\0';
                    l++;
                }
                printf("      to fit variable '%s'%s: %8.5f\n",
                       fVar[j]->GetTitle(), ws, corr);
            }

            printf("\n");
        }
    }

    //
    // calculate correlations between fit and control variables
    //

    if (fNVarCtrl)
    {
        // formatting stuff
        maxLen = 0;
        for (Int_t i = 0; i < fNVarCtrl; i++)
            maxLen = TMath::Max(maxLen, (Int_t)strlen(fVarCtrl[i]->GetTitle()));

        if (fNVar == 1) printf("\n");
        printf("  Fit-Control variable correlations\n");
        printf("\n");

        // loop over fit variables
        for (Int_t i = 0; i < fNVar; i++)
        {
            printf("    Fit variable '%s'\n", fVar[i]->GetTitle());

            // loop over control variables
            for (Int_t j = 0; j < fNVarCtrl; j++)
            {
                // calculate correlation
                Double_t corr = fData->correlation(*fVar[i], *fVarCtrl[j]);

                // format and print
                Int_t l = 0;
                for (Int_t k = strlen(fVarCtrl[j]->GetTitle()); k <= maxLen; k++)
                {
                    ws[l] = ' ';
                    ws[l+1] = '\0';
                    l++;
                }
                printf("      to control variable '%s'%s: %8.5f\n",
                       fVarCtrl[j]->GetTitle(), ws, corr);
            }

            printf("\n");
        }
    }

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFit::PostFit()
{
    // Perform tasks after fitting.
    // Return kTRUE on success, otherwise kFALSE.

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFit::Chi2PreFit()
{
    // Peform 'fNChi2PreFit' chi2 pre-fits randomizing the fit parameters
    // within their bounds. Afterwards set the parameters to the values
    // obtained in the fit that yielded the best chi2 value.
    // Return kTRUE on success, otherwise kFALSE.

    // check number of fits
    if (fNChi2PreFit <= 0)
    {
        Warning("Chi2PreFit", "No chi2 pre-fits were performed!");
        return kFALSE;
    }

    // create argument set of variables
    RooArgSet varSet;
    for (Int_t i = 0; i < fNVar; i++)
        varSet.add(*fVar[i]);

    // create a binned data set
    RooDataHist* dataBinned = new RooDataHist(TString::Format("%s_binned", fData->GetName()).Data(),
                                              TString::Format("%s (binned)", fData->GetTitle()).Data(),
                                              varSet,
                                              *fData);

    // get a list of the parameters
    RooArgSet* params = fModel->GetPdf()->getParameters(*fData);
    TIterator* iter = params->createIterator();

    // variables for best fit
    const Int_t nPar = params->getSize();
    Double_t bestChi2 = 1e99;
    Int_t bestFit = 0;
    Double_t bestPar[nPar];
    Double_t bestParErr[nPar];

    // user info
    Info("Chi2PreFit", "Performing %d binned chi2 pre-fit(s) to find "
                       "optimal %d fit parameters", fNChi2PreFit, nPar);

    // configure fit
    RooLinkedList fitArgs;
    fitArgs.Add(new RooCmdArg(RooFit::Extended()));
    fitArgs.Add(new RooCmdArg(RooFit::Save()));
    fitArgs.Add(new RooCmdArg(RooFit::Verbose(kFALSE)));
    fitArgs.Add(new RooCmdArg(RooFit::PrintLevel(-1)));
    fitArgs.Add(new RooCmdArg(RooFit::Warnings(kFALSE)));
    fitArgs.Add(new RooCmdArg(RooFit::PrintEvalErrors(-1)));
    fitArgs.Add(new RooCmdArg(CreateMinimizerArg(fMinimizerPreFit)));
    if (FFFooFit::gUseNCPU > 1)
        fitArgs.Add(new RooCmdArg(RooFit::NumCPU(FFFooFit::gUseNCPU, FFFooFit::gParStrat)));
    if (fRangeMin != 0 || fRangeMax != 0)
        fitArgs.Add(new RooCmdArg(RooFit::Range(fRangeMin, fRangeMax)));

    // perform a number of chi2 fits with random initial parameter values
    Int_t nFailed = 0;
    for (Int_t i = 0; i < fNChi2PreFit; i++)
    {
        // randomize parameters
        iter->Reset();
        while (RooRealVar* var = (RooRealVar*)iter->Next())
        {
            // check if parameter is constant
            if (!var->isConstant())
                var->randomize();
        }

        // perform chi2 fit
        RooFitResult* fit_res = fModel->GetPdf()->chi2FitTo(*dataBinned, fitArgs);

        // check fit result and repeat fit if it failed
        Bool_t fit_res_ok = CheckFitResult(fit_res, fMinimizerPreFit, kFALSE);
        delete fit_res;
        if (!fit_res_ok)
        {
            nFailed++;
            i--;
            continue;
        }

        // calculate chi2
        RooChi2Var chi2("prefit-chi2", "prefit-chi2", *fModel->GetPdf(), *dataBinned, RooFit::Extended());

        // print fit result
        printf("\n");
        printf("  Chi2 pre-fit %d    chi2 = %e\n\n", i+1, chi2.getVal());
        printf("  PARAMETER                          VALUE          ERROR\n");
        printf("  --------------------------------------------------------------\n");
        iter->Reset();
        while (RooRealVar* var = (RooRealVar*)iter->Next())
            printf("  %-32s  %13.6e  %13.6e\n", var->GetName(), var->getVal(), var->getError());
        printf("\n");

        // save best fit
        if (i == 0 || chi2.getVal() < bestChi2)
        {
            bestChi2 = chi2.getVal();
            bestFit = i;
            Int_t n = 0;
            iter->Reset();
            while (RooRealVar* var = (RooRealVar*)iter->Next())
            {
                bestPar[n] = var->getVal();
                bestParErr[n] = var->getError();
                n++;
            }
        }
    }

    // set parameters of best fit
    Int_t n = 0;
    iter->Reset();
    while (RooRealVar* var = (RooRealVar*)iter->Next())
    {
        var->setVal(bestPar[n]);
        var->setError(bestParErr[n]);
        n++;
    }

    // user info
    Info("Chi2PreFit", "Take fit parameters from fit %d with chi2 = %e", bestFit+1, bestChi2);
    Info("Chi2PreFit", "Failed binned chi2 pre-fits: %d", nFailed);
    Info("Chi2PreFit", "End of binned chi2 pre-fit(s)");

    // clean-up
    fitArgs.Delete();
    delete iter;
    delete params;
    delete dataBinned;

    return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooFit::Fit(const Char_t* opt)
{
    // Perform a RooFit-based fit.
    //
    // Options to be set via 'opt':
    // 'bchi2'      : perform a binned chi2 fit
    // 'nosumw2err' : set SumW2Error(kFALSE) for weighted fits
    //
    // Return kTRUE on success, otherwise kFALSE.

    // check fit variables
    if (!CheckVariables()) return kFALSE;

    // try to load the data
    if (!LoadData())
    {
        Error("Fit", "An error occurred during data loading!");
        return kFALSE;
    }

    // check data
    if (!fData)
    {
        Error("Fit", "No fit data found!");
        return kFALSE;
    }

    // check for weighted data set
    if (fData->isWeighted())
    {
        Info("Fit", "Using weighted dataset");

        // check for missing weights
        if (fData->sumEntries() == 0)
        {
            Error("Fit", "Missing weights - check weights configuration!");
            return kFALSE;
        }
    }

    // do various things before fitting
    if (!PrepareFit())
    {
        Error("Fit", "An error occurred while preparing the fit routine!");
        return kFALSE;
    }

    // check model
    if (!fModel)
    {
        Error("Fit", "No model found!");
        return kFALSE;
    }

    // build the model
    Info("Fit", "Building the model pdf");
    fModel->BuildModel(fVar, fNVar);

    // user info
    Info("Fit", "Fitting using %d CPU(s) (Parallelization strategy: %d)",
         FFFooFit::gUseNCPU, FFFooFit::gParStrat);

    // perform chi2 pre-fits
    if (fNChi2PreFit > 0)
    {
        if (!Chi2PreFit())
        {
            Error("Fit", "An error occurred in the chi2 pre-fit routine!");
            return kFALSE;
        }
    }

    // delete old fit result
    if (fResult)
        delete fResult;

    // fit the model to the data
    if (FFFooFit::IndexOf(opt, "bchi2") != -1)
    {
        Info("Fit", "Performing binned chi2 fit");

        // create argument set of variables
        RooArgSet varSet;
        for (Int_t i = 0; i < fNVar; i++) varSet.add(*fVar[i]);

        // create a binned data set
        RooDataHist* dataBinned = new RooDataHist(TString::Format("%s_binned", fData->GetName()).Data(),
                                                  TString::Format("%s (binned)", fData->GetTitle()).Data(),
                                                  varSet,
                                                  *fData);

        // configure fit
        RooLinkedList fitArgs;
        fitArgs.Add(new RooCmdArg(RooFit::Extended()));
        fitArgs.Add(new RooCmdArg(RooFit::Save()));
        fitArgs.Add(new RooCmdArg(RooFit::PrintEvalErrors(1)));
        fitArgs.Add(new RooCmdArg(CreateMinimizerArg(fMinimizer)));
        if (FFFooFit::gUseNCPU > 1)
            fitArgs.Add(new RooCmdArg(RooFit::NumCPU(FFFooFit::gUseNCPU, FFFooFit::gParStrat)));
        if (fRangeMin != 0 || fRangeMax != 0)
            fitArgs.Add(new RooCmdArg(RooFit::Range(fRangeMin, fRangeMax)));

        // perform binned chi2 fit
        fResult = fModel->GetPdf()->chi2FitTo(*dataBinned, fitArgs);

        // clean-up
        fitArgs.Delete();
        delete dataBinned;
    }
    else
    {
        Info("Fit", "Performing maximum likelihood fit");

        // look for additional constraints in the model
        TList* constrList = new TList();
        fModel->FindAllConstraints(constrList);
        TIter next(constrList);
        while (FFRooModel* c = (FFRooModel*)next())
            AddConstraint(c);
        delete constrList;

        // create argument set of constraints
        RooArgSet constrSet;
        for (Int_t i = 0; i < fNConstr; i++)
        {
            Info("Fit", "Using fit constraint %s", fConstr[i]->GetName());
            constrSet.add(*fConstr[i]->GetPdf());
        }

        // configure fit
        RooLinkedList fitArgs;
        fitArgs.Add(new RooCmdArg(RooFit::Extended()));
        fitArgs.Add(new RooCmdArg(RooFit::Save()));
        fitArgs.Add(new RooCmdArg(RooFit::PrintEvalErrors(1)));
        fitArgs.Add(new RooCmdArg(CreateMinimizerArg(fMinimizer)));
        if (fNConstr)
            fitArgs.Add(new RooCmdArg(RooFit::ExternalConstraints(constrSet)));
        if (FFFooFit::gUseNCPU > 1)
            fitArgs.Add(new RooCmdArg(RooFit::NumCPU(FFFooFit::gUseNCPU, FFFooFit::gParStrat)));
        if (fData->isWeighted())
        {
            if (FFFooFit::IndexOf(opt, "nosumw2err") != -1)
                fitArgs.Add(new RooCmdArg(RooFit::SumW2Error(kFALSE)));
            else
                fitArgs.Add(new RooCmdArg(RooFit::SumW2Error(kTRUE)));
        }
        if (fRangeMin != 0 || fRangeMax != 0)
            fitArgs.Add(new RooCmdArg(RooFit::Range(fRangeMin, fRangeMax)));

        // perform maximum likelihood fit
        fResult = fModel->GetPdf()->fitTo(*fData, fitArgs);

        // clean-up
        fitArgs.Delete();
    }

    // show fit result
    fResult->Print("v");

    // check fit result
    if (!CheckFitResult(fResult, fMinimizer))
        return kFALSE;

    // do various things after fitting
    if (!PostFit())
    {
        Error("Fit", "An error occurred during the post-fit routine!");
        return kFALSE;
    }

    return kTRUE;
}

//______________________________________________________________________________
RooPlot* FFRooFit::PlotDataAndModel(Int_t var, const Char_t* opt)
{
    // Plot the data and the model of the fit variable with index 'var'.
    //
    // Options:
    // l    : draw legend on left side (default: right)
    //
    // NOTE: the returned RooPlot has to be destroyed by the caller.

    // check variable index
    if (!CheckVarBounds(var, "PlotDataAndModel()")) return 0;

    // check for data
    if (!fData)
    {
        // try to load the data
        if (!LoadData())
        {
            Error("PlotDataAndModel", "An error occurred during data loading!");
            return 0;
        }
    }

    // create plot
    RooPlot* frame = fVar[var]->frame();

    // name plot
    frame->SetName(TString::Format("%s_Variable_%s", GetName(), fVar[var]->GetName()).Data());

    // plot data
    fData->plotOn(frame, RooFit::DataError(RooAbsData::SumW2));

    // check for model
    if (fModel && fModel->GetPdf())
    {
        // frame title
        frame->SetTitle(TString::Format("Variable '%s' of data '%s' fitted with model '%s'",
                                        fVar[var]->GetTitle(), fData->GetName(), fModel->GetTitle()).Data());

        // create legend
        TLegend* leg;
        if (!strcmp(opt, "l"))
            leg = new TLegend(0.1, 0.5, 0.3, 0.9);
        else
            leg = new TLegend(0.7, 0.5, 0.9, 0.9);
        leg->SetTextSize(0.03);
        leg->SetFillColor(0);
        leg->SetTextFont(42);

        // plot total model
        fModel->GetPdf()->plotOn(frame, RooFit::Name("plot_total_model"), RooFit::LineColor(kGreen));
        leg->AddEntry(frame->findObject("plot_total_model"), fModel->GetPdf()->GetTitle(), "l");

        // loop over components
        TIterator* iter = fModel->GetPdf()->getComponents()->createIterator();
        Int_t n = 0;
        while (RooAbsPdf* comp = (RooAbsPdf*)iter->Next())
        {
            // do not plot total pdf again
            if (comp == fModel->GetPdf())
                continue;

            // check if pdf has to contain the variable exclusively - if yes, skip component
            Bool_t drawVarExcl = kTRUE;
            if (comp->InheritsFrom("RooHistPdf") || comp->InheritsFrom("RooNDKeysPdf"))
                drawVarExcl = kFALSE;
            if (!ContainsVariable(comp, var, drawVarExcl))
                continue;

            // do not draw variable transformations
            if (comp->InheritsFrom("RooFormulaVar"))
                continue;

            // do not draw convolution components
            TString compName(comp->GetName());
            if (compName.EndsWith("_Conv_Intr") || compName.EndsWith("_Conv_Gauss"))
                continue;

            // plot component
            TString tmp = TString::Format("plot_model_%d", n);
            fModel->GetPdf()->plotOn(frame, RooFit::Name(tmp.Data()), RooFit::Components(*comp),
                                     RooFit::LineStyle(fgLStyle[n / 8]), RooFit::LineColor(fgColors[n % 8]));
            leg->AddEntry(frame->findObject(tmp.Data()), comp->GetTitle(), "l");

            // increment counter
            n++;
        }

        // resize legend
        leg->SetY1(0.9 - 0.055*n);

        // add legend
        frame->addObject(leg);

        // clean-up
        delete iter;
    }
    else
    {
        Warning("PlotDataAndModel", "No fitted model found - plotting data only");

        // frame title
        frame->SetTitle(TString::Format("Variable '%s' of data '%s'",
                                        fVar[var]->GetTitle(), fData->GetTitle()).Data());
    }

    return frame;
}

//______________________________________________________________________________
TH2* FFRooFit::PlotData2D(Int_t var0, Int_t var1)
{
    // Return a 2-dim. histogram containing the data of the variable 'var1'
    // plotted vs. the variable 'var0'.
    // NOTE: the returned histogram has to be destroyed by the caller.

    // check variable indices
    if (!CheckVarBounds(var0, "PlotData2D()")) return 0;
    if (!CheckVarBounds(var1, "PlotData2D()")) return 0;

    // check for data
    if (!fData)
    {
        // try to load the data
        if (!LoadData())
        {
            Error("PlotData2D", "An error occurred during data loading!");
            return 0;
        }
    }

    // create histogram
    TH2* h = (TH2*) fData->createHistogram(TString::Format("%s_vs_%s_data", fVar[var1]->GetName(), fVar[var0]->GetName()).Data(),
                                           *fVar[var0], RooFit::Binning(fVar[var0]->getBinning()),
                                           RooFit::YVar(*fVar[var1], RooFit::Binning(fVar[var1]->getBinning())));
    h->SetTitle(TString::Format("%s vs. %s (Data)", fVar[var1]->GetTitle(), fVar[var0]->GetTitle()).Data());

    return h;
}

//______________________________________________________________________________
TH2* FFRooFit::PlotModel2D(Int_t var0, Int_t var1)
{
    // Return a 2-dim. histogram containing the model of the variable 'var1'
    // plotted vs. the variable 'var0'.
    // NOTE: the returned histogram has to be destroyed by the caller.

    // check variable indices
    if (!CheckVarBounds(var0, "PlotModel2D()")) return 0;
    if (!CheckVarBounds(var1, "PlotModel2D()")) return 0;

    // check for model
    if (!fModel || !fModel->GetPdf())
    {
        Error("PlotModel2D", "No fitted model found!");
        return 0;
    }

    // create histogram
    TH2* h = (TH2*) fModel->GetPdf()->createHistogram(TString::Format("%s_vs_%s_model", fVar[var1]->GetName(), fVar[var0]->GetName()).Data(),
                                                      *fVar[var0], RooFit::Binning(fVar[var0]->getBinning()),
                                                      RooFit::YVar(*fVar[var1], RooFit::Binning(fVar[var1]->getBinning())));
    h->SetTitle(TString::Format("%s vs. %s (Model)", fVar[var1]->GetTitle(), fVar[var0]->GetTitle()).Data());

    return h;
}

//______________________________________________________________________________
TH1* FFRooFit::CreateDataHistogram(Int_t var)
{
    // Create and return a histogram of the data of variable 'var'.

    // check for data
    if (fData)
    {
        TH1* h = fData->createHistogram(fVar[var]->GetName());
        h->SetName(TString::Format("hist_%s", fVar[var]->GetName()).Data());
        return h;
    }
    else
    {
        Error("CreateDataHistogram", "No data to create histogram!");
        return 0;
    }
}

//______________________________________________________________________________
TCanvas* FFRooFit::DrawFit(const Char_t* opt, Int_t var)
{
    // Draw the fit results.
    // If 'var' is different to -1, plot only the variable with this index.
    // NOTE: the returned TCanvas has to be destroyed by the caller.

    // 1-dimensional fit
    if (fNVar == 1 || var != -1)
    {
        // create the canvas
        TCanvas* canvas;
        Int_t v;
        if (var != -1)
        {
            canvas = new TCanvas(TString::Format("%s_Result_Var_%d", GetName(), var).Data(),
                                 TString::Format("Result of %s (Variable %s)", GetTitle(), fVar[var]->GetTitle()).Data(),
                                 700, 500);
            v = var;
        }
        else
        {
            canvas = new TCanvas(TString::Format("%s_Result", GetName()).Data(),
                                 TString::Format("Result of %s", GetTitle()).Data(),
                                 700, 500);
            v = 0;
        }

        // plot data and model projection
        RooPlot* p = PlotDataAndModel(v, opt);
        if (p) p->Draw();

        return canvas;
    }
    // 2- and 3-dimensional fit
    else if (fNVar == 2 || fNVar == 3)
    {
        // create the canvas
        Int_t nPad = 1;
        TCanvas* canvas = new TCanvas(TString::Format("%s_Result", GetName()).Data(),
                                      TString::Format("Result of %s", GetTitle()).Data(),
                                      900, 800);

        if (fNVar == 2) canvas->Divide(2, 3);
        else if (fNVar == 3) canvas->Divide(3, 4);

        // plot data and model projections
        for (Int_t i = 0; i < fNVar; i++)
        {
            RooPlot* p = PlotDataAndModel(i, opt);
            canvas->cd(nPad++);
            if (p) p->Draw();
        }

        //
        // plot 2-dim. histograms
        //

        // combinations of variables
        Int_t var0[3] = { 0, 0, 1 };
        Int_t var1[3] = { 1, 2, 2 };
        Int_t n = 0;
        if (fNVar == 2) n = 1;
        else if (fNVar == 3) n = 3;

        // loop over combinations of variables
        for (Int_t i = 0; i < n; i++)
        {
            // create 2-dim. projections
            TH2* h01_d = PlotData2D(var0[i], var1[i]);
            TH2* h01_m = PlotModel2D(var0[i], var1[i]);

            // check projection histograms
            if (h01_d && h01_m)
            {
                // normalization and formatting
                h01_m->Scale(h01_d->Integral()/h01_m->Integral());
                h01_d->GetZaxis()->SetRangeUser(0, 1.05*h01_d->GetMaximum());
                h01_m->GetZaxis()->SetRangeUser(0, 1.05*h01_d->GetMaximum());

                // residue
                TH2* h01_r = (TH2*) h01_d->Clone(TString::Format("%s_vs_%s_residue",
                                                                 fVar[var1[i]]->GetName(), fVar[var0[i]]->GetName()).Data());
                h01_r->Add(h01_m, -1.);
                h01_r->SetTitle(TString::Format("%s vs. %s (Residue)",
                                                fVar[var1[i]]->GetTitle(), fVar[var0[i]]->GetTitle()).Data());

                // draw
                canvas->cd(nPad++);
                h01_d->Draw("colz");
                canvas->cd(nPad++);
                h01_m->Draw("colz");
                canvas->cd(nPad++);
                h01_r->Draw("colz");
            }
        }

        return canvas;
    }
    // fits in higher dimensions
    else
    {
        Error("DrawFit", "Drawing for %d fit variables not supported!", fNVar);
        return 0;
    }
}

//______________________________________________________________________________
TCanvas* FFRooFit::DrawCorrelations(const Char_t* opt)
{
    // Draw correlations between fit and control variables.
    // NOTE: the returned TCanvas has to be destroyed by the caller.

    // total number of variables
    Int_t nvar = fNVar + fNVarCtrl;

    // create the canvas
    TCanvas* canvas = new TCanvas(TString::Format("%s_Correl", GetName()).Data(),
                                  TString::Format("Variable correlations of %s", GetTitle()).Data(),
                                  700, 500);
    canvas->Divide(nvar, nvar, 0.001, 0.001);

    // create list of variables
    RooRealVar* vars[nvar];
    for (Int_t i = 0; i < fNVar; i++)
        vars[i] = fVar[i];
    for (Int_t i = 0; i < fNVarCtrl; i++)
        vars[fNVar + i] = fVarCtrl[i];

    // loop over variables
    Int_t n = 1;
    for (Int_t i = 0; i < nvar; i++)
    {
        // loop over variables
        for (Int_t j = 0; j < nvar; j++)
        {
            // create histogram
            TH2* h = (TH2*) fData->createHistogram(TString::Format("%s_vs_%s",
                                                   vars[j]->GetName(), vars[i]->GetName()).Data(),
                                                   *vars[i], RooFit::AutoBinning(100, 0.05),
                                                   RooFit::YVar(*vars[j], RooFit::AutoBinning(100, 0.05)));
            h->SetTitle(TString::Format("%s vs. %s", vars[j]->GetTitle(), vars[i]->GetTitle()).Data());

            // draw histogram
            canvas->cd(n);
            h->Draw(opt);

            // increment counter
            n++;
        }
    }

    return canvas;
}

