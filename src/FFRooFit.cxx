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


#include "RooRealVar.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH2.h"

#include "FFRooFit.h"
#include "FFFooFit.h"
#include "FFRooModel.h"

ClassImp(FFRooFit)

// init static class members
const Color_t FFRooFit::fgColors[8] = { kBlue, kRed, kMagenta, kCyan,
                                        kOrange-1, kGray, kYellow-6, 28 };

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
    fData = 0;
    fModel = 0;
    fResult = 0;
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
Bool_t FFRooFit::PreFit()
{
    // Perform tasks before fitting.
    // Return kTRUE on success, otherwise kFALSE.

    Char_t ws[256];
    Int_t maxLen;

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
Bool_t FFRooFit::Fit()
{
    // Perform a RooFit-based fit.
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

    // do various things before fitting
    if (!PreFit())
    {
        Error("Fit", "An error occurred during the pre-fit routine!");
        return kFALSE;
    }

    // check model
    if (!fModel)
    {
        Error("Fit", "No model found!");
        return kFALSE;
    }

    // build the model
    fModel->BuildModel(fVar);

    // user info
    Info("Fit", "Fitting using %d CPU(s) (Parallelization strategy: %d)",
         FFFooFit::gUseNCPU, FFFooFit::gParStrat);

    // fit the model to the data
    if (fResult) delete fResult;
    fResult = fModel->GetPdf()->fitTo(*fData, RooFit::Extended(),
                                              RooFit::Save(),
                                              FFFooFit::gUseNCPU > 1 ?
                                                  RooFit::NumCPU(FFFooFit::gUseNCPU, FFFooFit::gParStrat) :
                                                  RooCmdArg::none());

    // show fit result
    fResult->Print("v");

    // do various things after fitting
    if (!PostFit())
    {
        Error("Fit", "An error occurred during the post-fit routine!");
        return kFALSE;
    }

    return kTRUE;
}

//______________________________________________________________________________
RooPlot* FFRooFit::PlotDataAndModel(Int_t var)
{
    // Plot the data and the model of the fit variable with index 'var'.
    // NOTE: the returned RooPlot has to be destroyed by the caller.

    Char_t tmp[256];

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
    sprintf(tmp, "%s_Variable_%s", GetName(), fVar[var]->GetName());
    frame->SetName(tmp);

    // plot data
    fData->plotOn(frame, RooFit::DataError(RooAbsData::SumW2));

    // check for model
    if (fModel && fModel->GetPdf())
    {
        // frame title
        sprintf(tmp, "Variable '%s' of data '%s' fitted with model '%s'",
                fVar[var]->GetTitle(), fData->GetName(), fModel->GetTitle());
        frame->SetTitle(tmp);

        // create legend
        TLegend* leg = new TLegend(0.7, 0.5, 0.9, 0.9);
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
            if (comp == fModel->GetPdf()) continue;

            // check if pdf has to contain the variable exclusively - if yes, skip component
            Bool_t drawVarExcl = kTRUE;
            if (comp->InheritsFrom("RooHistPdf")) drawVarExcl = kFALSE;
            if (!ContainsVariable(comp, var, drawVarExcl)) continue;

            // plot component
            sprintf(tmp, "plot_model_%d", n);
            fModel->GetPdf()->plotOn(frame, RooFit::Name(tmp), RooFit::Components(*comp),
                                     RooFit::LineStyle(kDashed), RooFit::LineColor(fgColors[n%8]));
            leg->AddEntry(frame->findObject(tmp), comp->GetTitle(), "l");

            // increment counter
            n++;
        }

        // add legend
        frame->addObject(leg);

        // clean-up
        delete iter;
    }
    else
    {
        Warning("PlotDataAndModel", "No fitted model found - plotting data only");

        // frame title
        sprintf(tmp, "Variable '%s' of data '%s'", fVar[var]->GetTitle(), fData->GetTitle());
        frame->SetTitle(tmp);
    }

    return frame;
}

//______________________________________________________________________________
TH2* FFRooFit::PlotData2D(Int_t var0, Int_t var1)
{
    // Return a 2-dim. histogram containing the data of the variable 'var1'
    // plotted vs. the variable 'var0'.
    // NOTE: the returned histogram has to be destroyed by the caller.

    Char_t tmp[256];

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
    sprintf(tmp, "%s_vs_%s_data", fVar[var1]->GetName(), fVar[var0]->GetName());
    TH2* h = (TH2*) fData->createHistogram(tmp, *fVar[var0], RooFit::Binning(fVar[var0]->getBinning()),
                                           RooFit::YVar(*fVar[var1], RooFit::Binning(fVar[var1]->getBinning())));
    sprintf(tmp, "%s vs. %s (Data)", fVar[var1]->GetTitle(), fVar[var0]->GetTitle());
    h->SetTitle(tmp);

    return h;
}

//______________________________________________________________________________
TH2* FFRooFit::PlotModel2D(Int_t var0, Int_t var1)
{
    // Return a 2-dim. histogram containing the model of the variable 'var1'
    // plotted vs. the variable 'var0'.
    // NOTE: the returned histogram has to be destroyed by the caller.

    Char_t tmp[256];

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
    sprintf(tmp, "%s_vs_%s_model", fVar[var1]->GetName(), fVar[var0]->GetName());
    TH2* h = (TH2*) fModel->GetPdf()->createHistogram(tmp, *fVar[var0], RooFit::Binning(fVar[var0]->getBinning()),
                                                      RooFit::YVar(*fVar[var1], RooFit::Binning(fVar[var1]->getBinning())));
    sprintf(tmp, "%s vs. %s (Model)", fVar[var1]->GetTitle(), fVar[var0]->GetTitle());
    h->SetTitle(tmp);

    return h;
}

//______________________________________________________________________________
TCanvas* FFRooFit::DrawFit()
{
    // Draw the fit results.
    // NOTE: the returned TCanvas has to be destroyed by the caller.

    Char_t tmp[256];

    // 1-dimensional fit
    if (fNVar == 1)
    {
        // create the canvas
        sprintf(tmp, "Result of %s", GetTitle());
        TCanvas* canvas = new TCanvas(GetName(), tmp, 700, 500);

        // plot data and model projection
        RooPlot* p = PlotDataAndModel(0);
        if (p) p->Draw();

        return canvas;
    }
    // 2- and 3-dimensional fit
    else if (fNVar == 2 || fNVar == 3)
    {
        // create the canvas
        Int_t nPad = 1;
        sprintf(tmp, "Result of %s", GetTitle());
        TCanvas* canvas = new TCanvas(GetName(), tmp, 900, 800);
        if (fNVar == 2) canvas->Divide(2, 3);
        else if (fNVar == 3) canvas->Divide(3, 4);

        // plot data and model projections
        for (Int_t i = 0; i < fNVar; i++)
        {
            RooPlot* p = PlotDataAndModel(i);
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
                sprintf(tmp, "%s_vs_%s_residue", fVar[var1[i]]->GetName(), fVar[var0[i]]->GetName());
                TH2* h01_r = (TH2*) h01_d->Clone(tmp);
                h01_r->Add(h01_m, -1.);
                sprintf(tmp, "%s vs. %s (Residue)", fVar[var1[i]]->GetTitle(), fVar[var0[i]]->GetTitle());
                h01_r->SetTitle(tmp);

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

