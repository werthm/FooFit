/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2016
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooSPlot                                                           //
//                                                                      //
// Class for performing RooFit-based sPlot fits.                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TTree.h"
#include "TMath.h"
#include "RooStats/SPlot.h"
#include "RooAbsPdf.h"

#include "FFRooSPlot.h"
#include "FFRooModelSum.h"
#include "FFFooFit.h"

ClassImp(FFRooSPlot)

//______________________________________________________________________________
FFRooSPlot::FFRooSPlot(TTree* tree, Int_t nVar, Int_t nSpec,
                       const Char_t* name, const Char_t* title,
                       const Char_t* evIDVar, const Char_t* weightVar)
    : FFRooFitTree(tree, nVar, name, title, weightVar)
{
    // Constructor using the data tree 'tree' to fit 'nVar' variables of
    // 'nSpec' species.
    // The tree variable of the event ID can be specified with 'evIDVar'.
    // If 'weightVar' is non-zero, create a weighted dataset using this
    // tree variable to read the weights from.

    Char_t tmp[256];

    // init members
    fNSpec = nSpec;
    fSpecModel = new FFRooModel*[fNSpec];
    for (Int_t i = 0; i < fNSpec; i++) fSpecModel[i] = 0;
    fEventID = new RooRealVar(evIDVar, "Event ID", 0, 9.99999999999999e14);
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
    if (fTree->GetEntries() > max_int)
    {
        Error("FFRooSPlot", "Cannot process more than %lld entries!", max_int);
        fTree = 0;
    }

    // check event IDs
    if (!CheckEventID())
    {
        Error("FFRooSPlot", "Cannot process data tree due to event ID error!");
        fTree = 0;
    }

    // register event ID as auxiliary variable to import it into the dataset
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
    // Check if the event ID branch is present in the input data tree and
    // if all event ID numbers are valid.
    // Return kTRUE if everything is ok, otherwise kFALSE.

    // check if data contains an event ID branch
    if (!fTree->GetBranch(fEventID->GetName()))
    {
        Error("CheckEventID", "Event ID branch not found in data tree!");
        return kFALSE;
    }

    //
    // check if all event ID numbers do not have more than 15 significant digits
    //

    // read event ID
    Double_t event_id;
    fTree->SetBranchAddress(fEventID->GetName(), &event_id);

    // do not read other branches
    fTree->SetBranchStatus("*", 0);
    fTree->SetBranchStatus(fEventID->GetName(), 1);

    // user info
    Info("CheckEventID", "Checking event IDs of %.9e events...", (Double_t)fTree->GetEntries());

    Int_t nerror = 0;
    for (Long64_t i = 0; i < fTree->GetEntries(); i++)
    {
        // read entry
        fTree->GetEntry(i);

        // check limit
        if (TMath::Log10(event_id) >= 15)
        {
            Error("CheckEventID", "Event ID %.15g of event %lld above limit!", event_id, i);
            nerror++;
        }

        // abort when too many errors
        if (nerror > 10)
        {
            Error("CheckEventID", "Too many invalid event IDs - aborting scan!");
            break;
        }
    }

    // reset branch addresses
    fTree->ResetBranchAddresses();
    fTree->SetBranchStatus("*", 1);

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
const Char_t* FFRooSPlot::GetSpeciesName(Int_t i) const
{
    // Return the name of the species at index 'i'.

    // check species index
    if (CheckSpecBounds(i, "GetSpeciesName()"))
        return fModel->GetParTitle(i);
    else
        return 0;
}

//______________________________________________________________________________
Double_t FFRooSPlot::GetSpeciesYield(Int_t i) const
{
    // Return the yield of the species at index 'i'.

    // check species index
    if (CheckSpecBounds(i, "GetSpeciesYield()"))
        return fModel->GetParameter(i);
    else
        return 0;
}

//______________________________________________________________________________
Double_t FFRooSPlot::GetSpeciesYieldError(Int_t i) const
{
    // Return the yield error of the species at index 'i'.

    // check species index
    if (CheckSpecBounds(i, "GetSpeciesYieldError()"))
        return fModel->GetParError(i);
    else
        return 0;
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
Bool_t FFRooSPlot::Fit(const Char_t* opt)
{
    // Perform the sPlot fit.
    //
    // Options to be set via 'opt':
    // 'nosplot'     : skip the sPlot fit (only perform first-level fit)
    //
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
    if (!FFRooFitTree::Fit(opt))
    {
        Error("Fit", "An error occurred during the first level fit!");
        return kFALSE;
    }

    // check for skipping sPlot fit
    if (FFFooFit::IndexOf(opt, "nosplot") != -1)
        return kTRUE;

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
                                 fModel->GetPdf(), yieldParList, RooArgSet(), kTRUE, kTRUE);

    // user info
    for (Int_t i = 0; i < fNSpec; i++)
    {
        Info("Fit", "Species '%s' yield: %e (%s)  %e (SPlot)",
             fModel->GetParTitle(i), fModel->GetParameter(i), fModel->GetParName(i),
             fSPlot->GetYieldFromSWeight(fModel->GetParName(i)));
    }

    return kTRUE;
}

//______________________________________________________________________________
Int_t FFRooSPlot::GetNEvents() const
{
    // Return the number of fitted events.

    if (fSPlot)
        return fSPlot->GetSDataSet()->numEntries();
    else
        return 0;
}

//______________________________________________________________________________
Double_t FFRooSPlot::GetEventID(Int_t event) const
{
    // Return the ID of the event 'event'.

    if (fSPlot)
        return fSPlot->GetSDataSet()->get(event)->getRealValue(fEventID->GetName());
    else
        return 0;
}

//______________________________________________________________________________
Double_t FFRooSPlot::GetSpeciesWeight(Int_t event, Int_t i) const
{
    // Return the weight of the species at index 'i' for the event 'event'.

    // check species index
    if (CheckSpecBounds(i, "GetSpeciesWeight()"))
    {
        if (fSPlot)
            return fSPlot->GetSWeight(event, fModel->GetParName(i));
        else
            return 0;
    }
    else return 0;
}

//______________________________________________________________________________
TTree* FFRooSPlot::GetSpeciesWeightsTree(const Char_t* name,
                                         Int_t nSpec, Int_t* spec) const
{
    // Return a tree named 'name' containing the species weights for all events.
    // If 'nSpec' and 'spec' are non-zero, save only the weights for the 'nSpec'
    // species with indices 'spec' to the tree.

    // check fit
    if (!fSPlot)
        return 0;

    // number of events
    Int_t nEvents = GetNEvents();

    // check species
    Int_t specIdx[nSpec];
    if (!nSpec && !spec)
    {
        for (Int_t i = 0; i < fNSpec; i++)
            specIdx[i] = i;
        nSpec = fNSpec;
        spec = specIdx;
    }

    // set tree name
    TString tname;
    if (name)
        tname = name;
    else
        tname = TString::Format("%s_sweights", GetName());

    // create the tree title listing the species
    TString ttitle = "Species: ";
    for (Int_t i = 0; i < nSpec; i++)
    {
        ttitle += GetSpeciesName(spec[i]);
        ttitle += TString::Format("(%d)", spec[i]);
        if (i != nSpec-1) ttitle += ",";
    }

    // create the tree
    TTree* tree = new TTree(tname.Data(), ttitle.Data());

    // create branches
    Double_t event_id;
    Float_t weights[nSpec];
    tree->Branch("event_id", &event_id, "event_id/D");
    for (Int_t i = 0; i < nSpec; i++)
    {
        tree->Branch(TString::Format("sweights_%d", spec[i]).Data(),
                     &weights[i],
                     TString::Format("sweights_%d/F", spec[i]).Data());
    }

    // loop over fitted events
    for (Int_t i = 0; i < nEvents; i++)
    {
        // read event ID
        event_id = GetEventID(i);

        // loop over species
        for (Int_t j = 0; j < nSpec; j++)
        {
            // get weight for species
            weights[j] = GetSpeciesWeight(i, spec[j]);
        }

        // fill tree
        tree->Fill();
    }

    return tree;
}

