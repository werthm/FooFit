/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooSPlot                                                           //
//                                                                      //
// Class for performing RooFit-based sPlot fits.                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TChain.h"
#include "TMath.h"
#include "RooStats/SPlot.h"
#include "RooAbsPdf.h"

#include "FFRooSPlot.h"
#include "FFRooModelSum.h"

ClassImp(FFRooSPlot)

// init static class members
const Char_t FFRooSPlot::fgBranchEventID[] = "event_id";

//______________________________________________________________________________
FFRooSPlot::FFRooSPlot(TChain* chain, Int_t nVar, Int_t nSpec)
    : FFRooFitTree(chain, nVar)
{
    // Constructor using the data chain 'chain' to fit 'nVar' variables of
    // 'nSpec' species.

    Char_t tmp[256];

    // init members
    fNSpec = nSpec;
    fSpecModel = new FFRooModel*[fNSpec];
    for (Int_t i = 0; i < fNSpec; i++) fSpecModel[i] = 0;
    fEventID = new RooRealVar(fgBranchEventID, "Event ID", 0, 9.99999999999999e14);
    fSPlot = 0;

    // create model
    fModel = new FFRooModelSum("total_model", "total model", fNSpec);

    // overwrite parameter names
    for (Int_t i = 0; i < fNSpec; i++)
    {
        sprintf(tmp, "Yield_Species_%d", i);
        fModel->SetParName(i, tmp);
        sprintf(tmp, "Species_%d", i);
        fModel->SetParTitle(i, tmp);
    }

    // check number of entries (RooDataSet uses Int_t as entry number)
    const Long64_t max_int = ~(1 << 31);
    if (fChain->GetEntries() > max_int)
    {
        Error("FFRooSPlot", "Cannot process more than %lld entries!", max_int);
        fChain = 0;
    }

    // check event ids
    if (!CheckEventID())
    {
        Error("FFRooSPlot", "Cannot process data chain due to event id error!");
        fChain = 0;
    }

    // register event id as auxiliary variable to import it into the dataset
    AddAuxVariable(fEventID);
}

//______________________________________________________________________________
FFRooSPlot::~FFRooSPlot()
{
    // Destructor.

    // delete locally owned parent member
    if (fModel) delete fModel;

    if (fSpecModel) delete [] fSpecModel;
    if (fEventID) delete fEventID;
    if (fSPlot) delete fSPlot;
}

//______________________________________________________________________________
Bool_t FFRooSPlot::CheckEventID()
{
    // Check if the event id branch is present in the input data chain and
    // if all event id numbers are valid.
    // Return kTRUE if everything is ok, otherwise kFALSE.

    // check if data contains an event id branch
    if (!fChain->GetBranch(fgBranchEventID))
    {
        Error("CheckEventID", "Event id branch not found in data chain!");
        return kFALSE;
    }

    //
    // check if all event id numbers do not have more than 15 significant digits
    //

    // read event id
    Double_t event_id;
    fChain->SetBranchAddress(fgBranchEventID, &event_id);

    // do not read other branches
    fChain->SetBranchStatus("*", 0);
    fChain->SetBranchStatus(fgBranchEventID, 1);

    // user info
    Info("CheckEventID", "Checking event ids of %.9e events...", (Double_t)fChain->GetEntries());

    Int_t nerror = 0;
    for (Long64_t i = 0; i < fChain->GetEntries(); i++)
    {
        // read entry
        fChain->GetEntry(i);

        // check limit
        if (TMath::Log10(event_id) >= 15)
        {
            Error("CheckEventID", "Event id %.15g of event %lld above limit!", event_id, i);
            nerror++;
        }

        // abort when too many errors
        if (nerror > 10)
        {
            Error("CheckEventID", "Too many invalid event ids - aborting scan!");
            break;
        }
    }

    // reset branch addresses
    fChain->ResetBranchAddresses();
    fChain->SetBranchStatus("*", 1);

    // check errors
    if (nerror) return kFALSE;
    else return kTRUE;
}

//______________________________________________________________________________
Bool_t FFRooSPlot::CheckSpecBounds(Int_t spec, const Char_t* loc) const
{
    // Check if the species index 'spec' is within valid bounds.
    // Return kTRUE if the index 'spec' is valid, otherwise kFALSE.
    // Use 'loc' to set the location of the error.

    // check index range
    if (spec < 0 || spec >= fNSpec)
    {
        Error("CheckSpecBounds", "%s: Invalid species index %d (number of species: %d)",
              loc, spec, fNSpec);
        return kFALSE;
    }
    else return kTRUE;
}

//______________________________________________________________________________
Double_t FFRooSPlot::GetSpeciesWeight(Int_t i, Int_t event) const
{
    // Return the weight of the species at index 'i' for the event 'event'.

    // check species index
    if (CheckSpecBounds(i, "GetSpeciesWeight()"))
    {
        if (fSPlot) fSPlot->GetSWeight(event, fModel->GetParName(i));
    }
}

//______________________________________________________________________________
void FFRooSPlot::SetSpeciesYield(Int_t i, Double_t v)
{
    // Set the yield for the species at index 'i' to the value 'v'.

    // check species index
    if (CheckSpecBounds(i, "SetSpeciesYield()"))
    {
        fModel->SetParameter(i, v);
    }
}

//______________________________________________________________________________
void FFRooSPlot::SetSpeciesYield(Int_t i, Double_t v, Double_t min, Double_t max)
{
    // Set the yield for the species at index 'i' to the value 'v' and set the
    // range to the interval [min,max].

    // check species index
    if (CheckSpecBounds(i, "SetSpeciesYield()"))
    {
        fModel->SetParameter(i, v, min, max);
    }
}

//______________________________________________________________________________
void FFRooSPlot::SetSpeciesYieldLimits(Int_t i, Double_t min, Double_t max)
{
    // Set the range of the yield for the species at index 'i' to the interval [min,max].

    // check species index
    if (CheckSpecBounds(i, "SetSpeciesYieldLimits()"))
    {
        fModel->SetParLimits(i, min, max);
    }
}

//______________________________________________________________________________
void FFRooSPlot::SetSpeciesName(Int_t i, const Char_t* name)
{
    // Set the name of the species at index 'i' to 'name'.

    // check species index
    if (CheckSpecBounds(i, "SetSpeciesName()"))
    {
        Char_t tmp[256];
        sprintf(tmp, "Yield_%s", name);
        fModel->SetParName(i, tmp);
        fModel->SetParTitle(i, name);
    }
}

//______________________________________________________________________________
void FFRooSPlot::SetSpeciesModel(Int_t i, FFRooModel* model)
{
    // Set the model for the species at index 'i' to 'model'.

    // check species index
    if (CheckSpecBounds(i, "SetSpeciesModel()"))
    {
        fSpecModel[i] = model;
    }
}

//______________________________________________________________________________
Bool_t FFRooSPlot::Fit()
{
    // Perform the sPlot fit.
    // Return kTRUE on success, otherwise kFALSE.

    // check species models
    for (Int_t i = 0; i < fNSpec; i++)
    {
        if (!fSpecModel[i])
        {
            Error("Fit", "No model was added for species %d!", i);
            return kFALSE;
        }
    }

    // set species models in total model list
    ((FFRooModelSum*)fModel)->SetModelList(fSpecModel);

    // perform first level fit
    if (!FFRooFitTree::Fit())
    {
        Error("Fit", "An error occurred during the first level fit!");
        return kFALSE;
    }

    // fix all parameters for the sPlot fit except the fit variables and species yields
    TIterator* iter = fModel->GetPdf()->getVariables()->createIterator();
    while (RooRealVar* var = (RooRealVar*)iter->Next())
    {
        Bool_t skip = kFALSE;

        // check for fit variable
        for (Int_t i = 0; i < fNVar; i++)
        {
            if (var == fVar[i])
            {
                skip = kTRUE;
                break;
            }
        }
        if (skip) continue;

        // check for yield parameter
        for (Int_t i = 0; i < fNSpec; i++)
        {
            if (var == fModel->GetPar(i))
            {
                skip = kTRUE;
                break;
            }
        }
        if (skip) continue;

        // fix parameter
        var->setConstant();
        Info("Fit", "Fixing parameter '%s' for sPlot fit", var->GetName());
    }

    // clean-up
    delete iter;

    // create argument set of species yield parameters
    RooArgList yieldParList;
    for (Int_t i = 0; i < fNSpec; i++) yieldParList.add(*fModel->GetPar(i));

    // create the sPlot object
    fSPlot = new RooStats::SPlot("splot_fit", "FFRooSPlot Fit", *((RooDataSet*)fData),
                                 fModel->GetPdf(), yieldParList);

    // user info
    for (Int_t i = 0; i < fNSpec; i++)
    {
        Info("Fit", "Species '%s' yield: %e (%s)  %e (SPlot)",
             fModel->GetParTitle(i), fModel->GetParameter(i), fModel->GetParName(i),
             fSPlot->GetYieldFromSWeight(fModel->GetParName(i)));
    }

    return kTRUE;
}

