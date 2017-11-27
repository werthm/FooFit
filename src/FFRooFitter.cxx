/*************************************************************************
 * Author: Dominik Werthmueller, 2017
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitter                                                          //
//                                                                      //
// Abstract class for fitting multiple species to data.                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "FFRooFitter.h"
#include "FFRooModelSum.h"
#include "FFRooFitterSpecies.h"

ClassImp(FFRooFitter)

//______________________________________________________________________________
FFRooFitter::FFRooFitter(const Char_t* name, const Char_t* title)
    : TNamed(name, title)
{
    // Constructor.

    // init members
    fFitter = 0;
    fModel = 0;
    fNSpec = 0;
    fSpec = 0;
}

//______________________________________________________________________________
FFRooFitter::~FFRooFitter()
{
    // Destructor.

    if (fFitter) delete fFitter;
    if (fModel) delete fModel;
    if (fSpec)
    {
        for (Int_t i = 0; i < fNSpec; i++)
            if (fSpec[i]) delete fSpec[i];
        delete [] fSpec;
    }
}

//______________________________________________________________________________
void FFRooFitter::AddSpecies(FFRooFitterSpecies* spec)
{
    // Add the species 'spec' to the list of species to fit.
    // Note: The object is not copied into this class, only a pointer is set.

    // backup old list
    FFRooFitterSpecies** old = fSpec;

    // create new list
    fSpec = new FFRooFitterSpecies*[fNSpec+1];
    for (Int_t i = 0; i < fNSpec; i++) fSpec[i] = old[i];

    // add new element
    fSpec[fNSpec] = spec;
    fNSpec++;

    // destroy old list
    if (old) delete [] old;
}

//______________________________________________________________________________
FFRooFitterSpecies* FFRooFitter::GetSpecies(Int_t i) const
{
    // Return the species with index 'i'.

    if (i >= 0 && i < fNSpec)
        return fSpec[i];
    else
        return 0;
}

//______________________________________________________________________________
void FFRooFitter::SetVariable(Int_t i, const Char_t* name, const Char_t* title,
                              Double_t min, Double_t max, Int_t nbins)
{
    // Wrapper for FFRooFit::SetVariable().

    if (fFitter)
        fFitter->SetVariable(i, name, title, min, max, nbins);
    else
        Error("SetVariable", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::AddAuxVariable(RooRealVar* aux_var)
{
    // Wrapper for FFRooFit::AddAuxVariable().

    if (fFitter)
        fFitter->AddAuxVariable(aux_var);
    else
        Error("AddAuxVariable", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::AddControlVariable(const Char_t* name, const Char_t* title)
{
    // Wrapper for FFRooFit::AddControlVariable().

    if (fFitter)
        fFitter->AddControlVariable(name, title);
    else
        Error("AddControlVariable", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::SetNChi2PreFit(Int_t n)
{
    // Wrapper for FFRooFit::SetNChi2PreFit().

    if (fFitter)
        fFitter->SetNChi2PreFit(n);
    else
        Error("SetNChi2PreFit", "Fitter not created yet!");
}

//______________________________________________________________________________
void FFRooFitter::SetMinimizer(FFRooFit::FFMinimizer_t min)
{
    // Wrapper for FFRooFit::SetMinimizer().

    if (fFitter)
        fFitter->SetMinimizer(min);
    else
        Error("SetMinimizer", "Fitter not created yet!");
}

//______________________________________________________________________________
TCanvas* FFRooFitter::DrawFit(const Char_t* opt, Int_t var)
{
    // Wrapper for FFRooFit::DrawFit().

    if (fFitter)
        return fFitter->DrawFit(opt, var);
    else
        Error("DrawFit", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
TCanvas* FFRooFitter::DrawCorrelations(const Char_t* opt)
{
    // Wrapper for FFRooFit::DrawCorrelations().

    if (fFitter)
        return fFitter->DrawCorrelations(opt);
    else
        Error("DrawCorrelations", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
RooPlot* FFRooFitter::PlotDataAndModel(Int_t var, const Char_t* opt)
{
    // Wrapper for FFRooFit::PlotDataAndModel().

    if (fFitter)
        return fFitter->PlotDataAndModel(var, opt);
    else
        Error("PlotDataAndModel", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
TH2* FFRooFitter::PlotData2D(Int_t var0, Int_t var1)
{
    // Wrapper for FFRooFit::PlotData2D().

    if (fFitter)
        return fFitter->PlotData2D(var0, var1);
    else
        Error("PlotData2D", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
TH2* FFRooFitter::PlotModel2D(Int_t var0, Int_t var1)
{
    // Wrapper for FFRooFit::PlotModel2D().

    if (fFitter)
        return fFitter->PlotModel2D(var0, var1);
    else
        Error("PlotModel2D", "Fitter not created yet!");

    return 0;
}

//______________________________________________________________________________
Bool_t FFRooFitter::BuildModel()
{
    // Build the total model.
    // Return kFALSE if an error occurred, otherwise kTRUE

    if (fFitter)
    {
        // create total model
        fModel = new FFRooModelSum("total_model", "total model", fNSpec);

        // configure models of all species
        for (Int_t i = 0; i < fNSpec; i++)
        {
            // add model
            ((FFRooModelSum*)fModel)->SetModel(i, fSpec[i]->GetModel());

            // parameter name and title
            fModel->SetParName(i, TString::Format("Yield_%s", fSpec[i]->GetName()));
            fModel->SetParTitle(i, TString::Format("Yield of species '%s'", fSpec[i]->GetTitle()));

            // configure yield parameter
            Double_t init = fSpec[i]->GetYieldInit();
            Double_t min = fSpec[i]->GetYieldMin();
            Double_t max = fSpec[i]->GetYieldMax();
            if (init != 0 || min != 0 || max != 0)
                fModel->SetParameter(i, init, min, max);
        }

        // set total model
        fFitter->SetModel(fModel);

        return kTRUE;
    }
    else
    {
        Error("BuildModel", "Fitter not created yet!");
        return kFALSE;
    }
}

//______________________________________________________________________________
Bool_t FFRooFitter::Fit(const Char_t* opt)
{
    // Perform the fit.

    // build the total model
    if (!BuildModel())
    {
        Error("Fit", "An error occurred while building the fit model!");
        return kFALSE;
    }

    // perform the fit
    Bool_t res = fFitter->Fit(opt);

    // copy yield parameters
    for (Int_t i = 0; i < fNSpec; i++)
    {
        fSpec[i]->SetYieldFit(fModel->GetParameter(i));
        fSpec[i]->SetYieldFitError(fModel->GetParError(i));
        fSpec[i]->UpdateModelParameters();
    }

    return res;
}

