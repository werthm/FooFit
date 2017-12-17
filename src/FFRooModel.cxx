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


#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include "FFRooModel.h"
#include "FFRooModelComp.h"
#include "FFRooModelGauss.h"

ClassImp(FFRooModel)

//______________________________________________________________________________
FFRooModel::FFRooModel(const Char_t* name, const Char_t* title, Int_t nPar)
    : TNamed(name, title)
{
    // Constructor.

    // init members
    fPdf = 0;
    fNPar = nPar;
    fPar = new RooRealVar*[fNPar];
    fParIsOwned = new Bool_t[fNPar];
    for (Int_t i = 0; i < fNPar; i++)
    {
        fPar[i] = 0;
        fParIsOwned[i] = kFALSE;
    }
    fNVarTrans = 0;
    fVarTrans = 0;
    fNConstr = 0;
    fConstr = 0;
}

//______________________________________________________________________________
FFRooModel::~FFRooModel()
{
    // Destructor.

    if (fPdf) delete fPdf;
    if (fPar)
    {
        for (Int_t i = 0; i < fNPar; i++)
            if (fPar[i] && fParIsOwned[i]) delete fPar[i];
        delete [] fPar;
    }
    if (fVarTrans)
    {
        for (Int_t i = 0; i < fNVarTrans; i++)
            if (fVarTrans[i]) delete fVarTrans[i];
        delete [] fVarTrans;
    }
    if (fConstr)
    {
        for (Int_t i = 0; i < fNConstr; i++)
            if (fConstr[i]) delete fConstr[i];
        delete [] fConstr;
    }
}

//______________________________________________________________________________
Bool_t FFRooModel::CheckParBounds(Int_t par, const Char_t* loc) const
{
    // Check if the parameter index 'par' is within valid bounds.
    // Return kTRUE if the index 'par' is valid, otherwise kFALSE.
    // Use 'loc' to set the location of the error.

    // check index range
    if (par < 0 || par >= fNPar)
    {
        Error("CheckParBounds", "%s: Invalid parameter index %d (number of parameters: %d)",
              loc, par, fNPar);
        return kFALSE;
    }
    else return kTRUE;
}

//______________________________________________________________________________
void FFRooModel::AddParameter(Int_t i, const Char_t* name, const Char_t* title)
{
    // Add a parameter at index 'i' using the name 'name' and title 'title'.

    // check parameter index
    if (CheckParBounds(i, "AddParameter()"))
    {
        if (fPar[i] && fParIsOwned[i])
            delete fPar[i];
        fPar[i] = new RooRealVar(name, title, -RooNumber::infinity(), RooNumber::infinity());
        fParIsOwned[i] = kTRUE;
    }
}

//______________________________________________________________________________
void FFRooModel::AddVarTrans(RooAbsReal* varTrans)
{
    // Add the variable transformation 'varTrans'.
    // Note: The object is not copied into this class, only a pointer is set.

    // backup old list
    RooAbsReal** old = fVarTrans;

    // create new list
    fVarTrans = new RooAbsReal*[fNVarTrans+1];
    for (Int_t i = 0; i < fNVarTrans; i++) fVarTrans[i] = old[i];

    // add new element
    fVarTrans[fNVarTrans] = varTrans;
    fNVarTrans++;

    // destroy old list
    if (old) delete [] old;
}

//______________________________________________________________________________
void FFRooModel::AddConstraint(FFRooModel* c)
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
RooRealVar* FFRooModel::GetPar(Int_t i) const
{
    // Return the parameter at index 'i'.

    // check parameter index
    if (CheckParBounds(i, "GetPar()")) return fPar[i];
    else return 0;
}

//______________________________________________________________________________
Double_t FFRooModel::GetParameter(Int_t i) const
{
    // Return the value of the parameter at index 'i'.

    // check parameter index
    if (CheckParBounds(i, "GetParameter()"))
    {
        return fPar[i] ? fPar[i]->getVal() : 0;
    }
    else return 0;
}

//______________________________________________________________________________
Double_t FFRooModel::GetParError(Int_t i) const
{
    // Return the error of the value of the parameter at index 'i'.

    // check parameter index
    if (CheckParBounds(i, "GetParameter()"))
    {
        return fPar[i] ? fPar[i]->getError() : 0;
    }
    else return 0;
}

//______________________________________________________________________________
Bool_t FFRooModel::IsParConstant(Int_t i) const
{
    // Check if the parameter at index 'i' is marked as constant.

    // check parameter index
    if (CheckParBounds(i, "IsParConstant()"))
    {
        return fPar[i] ? fPar[i]->isConstant() : kFALSE;
    }
    else return kFALSE;
}

//______________________________________________________________________________
const Char_t* FFRooModel::GetParName(Int_t i) const
{
    // Return the name of the parameter at index 'i'.

    // check parameter index
    if (CheckParBounds(i, "GetParName()"))
    {
        return fPar[i] ? fPar[i]->GetName() : 0;
    }
    else return 0;
}

//______________________________________________________________________________
const Char_t* FFRooModel::GetParTitle(Int_t i) const
{
    // Return the title of the parameter at index 'i'.

    // check parameter index
    if (CheckParBounds(i, "GetParTitle()"))
    {
        return fPar[i] ? fPar[i]->GetTitle() : 0;
    }
    else return 0;
}

//______________________________________________________________________________
void FFRooModel::SetParameter(Int_t i, RooRealVar* par)
{
    // Set the parameter at index 'i' to the external (not owned) parameter 'par'.

    // delete old parameter
    if (fPar[i] && fParIsOwned[i])
        delete fPar[i];

    // set external parameter
    fPar[i] = par;
    fParIsOwned[i] = kFALSE;
}

//______________________________________________________________________________
void FFRooModel::SetParameter(Int_t i, Double_t v)
{
    // Set the value of the parameter at index 'i' to 'v'.

    // check parameter index
    if (CheckParBounds(i, "SetParameter()"))
    {
        if (fPar[i]) fPar[i]->setVal(v);
    }
}

//______________________________________________________________________________
void FFRooModel::SetParameter(Int_t i, Double_t v, Double_t min, Double_t max)
{
    // Set the value of the parameter at index 'i' to 'v' and set the range
    // to the interval [min,max].

    // check parameter index
    if (CheckParBounds(i, "SetParameter()"))
    {
        if (fPar[i])
        {
            // check if parameter should be fixed
            if (v == min && v == max)
            {
                Warning("SetParameter", "Setting parameter %s to a constant value of %f",
                        fPar[i]->GetName(), v);
                fPar[i]->setVal(v);
                fPar[i]->setConstant(kTRUE);
            }
            else
            {
                fPar[i]->setVal(v);
                fPar[i]->setRange(min, max);
            }
        }
    }
}

//______________________________________________________________________________
void FFRooModel::SetParLimits(Int_t i, Double_t min, Double_t max)
{
    // Set the range of the parameter at index 'i' to the interval [min,max].

    // check parameter index
    if (CheckParBounds(i, "SetParLimits()"))
    {
        if (fPar[i])
        {
            // check if parameter should be fixed
            if (min == max)
            {
                Warning("SetParLimits", "Setting parameter %s to a constant value of %f",
                        fPar[i]->GetName(), min);
                fPar[i]->setVal(min);
                fPar[i]->setConstant(kTRUE);
            }
            else
            {
                fPar[i]->setRange(min, max);
            }
        }
    }
}

//______________________________________________________________________________
void FFRooModel::SetParName(Int_t i, const Char_t* name)
{
    // Set the name of the parameter at index 'i' to 'name'.

    // check parameter index
    if (CheckParBounds(i, "SetParName()"))
    {
        if (fPar[i]) fPar[i]->SetName(name);
    }
}

//______________________________________________________________________________
void FFRooModel::SetParTitle(Int_t i, const Char_t* title)
{
    // Set the title of the parameter at index 'i' to 'title'.

    // check parameter index
    if (CheckParBounds(i, "SetParTitle()"))
    {
        if (fPar[i]) fPar[i]->SetTitle(title);
    }
}

//______________________________________________________________________________
void FFRooModel::FixParameter(Int_t i, Double_t v)
{
    // Fix the value of the parameter at index 'i' to 'v'.

    // check parameter index
    if (CheckParBounds(i, "FixParameter()"))
    {
        if (fPar[i])
        {
            fPar[i]->setVal(v);
            fPar[i]->setConstant(kTRUE);
        }
    }
}

//______________________________________________________________________________
void FFRooModel::AddParConstrGauss(Int_t i, Double_t mean, Double_t sigma)
{
    // Add a Gaussian fit constraint to the parameter with index 'i' using
    // the 'mean' and 'sigma' values.

    // check parameter index
    if (!CheckParBounds(i, "AddParConstrGauss()"))
        return;

    // create constraint model
    FFRooModelGauss* c = new FFRooModelGauss(TString::Format("Gauss_Constr_Par_%s",
                                                             fPar[i]->GetName()).Data(),
                                             TString::Format("Gaussian constraint on parameter %s",
                                                             fPar[i]->GetTitle()).Data());

    // set constraint parameters
    c->FixParameter(0, mean);
    c->FixParameter(1, sigma);

    // build the constraint model
    c->BuildModel(fPar + i);

    // add constraint
    AddConstraint(c);
}

//______________________________________________________________________________
void FFRooModel::FindAllConstraints(TList* list)
{
    // Recursive method to find all constraints that were added to this model.
    // All constraints will be added to the list 'list'.

    // loop over constraints
    for (Int_t i = 0; i < fNConstr; i++)
        list->Add(fConstr[i]);

    // check for models with sub-models
    if (this->InheritsFrom("FFRooModelComp"))
    {
        FFRooModelComp* m = (FFRooModelComp*)this;

        // loop over sub-models
        for (Int_t i = 0; i < m->GetNModel(); i++)
            m->GetModel(i)->FindAllConstraints(list);
    }
}

//______________________________________________________________________________
void FFRooModel::Print(Option_t* option) const
{
    // Print out the content of this class.

    printf("%sFFRooModel content:\n", option);
    printf("%sName                       : %s\n", option, GetName());
    printf("%sTitle                      : %s\n", option, GetTitle());
    printf("%sNumber of parameters       : %d\n", option, fNPar);
    if (fNPar)
    {
        printf("%sParameter owned            : ", option);
        for (Int_t i = 0; i < fNPar; i++)
        {
            if (fParIsOwned[i])
                printf("yes");
            else
                printf("no");
            if (i != fNPar-1)
                printf(", ");
        }
        printf("\n");
    }
    printf("%sNumber of variable transf. : %d\n", option, fNVarTrans);
    printf("%sNumber of constraints      : %d\n", option, fNConstr);
    printf("%sModel PDF                  : 0x%x\n", option, fPdf);
    if (fNPar)
    {
        printf("%sParameters\n", option);
        for (Int_t i = 0; i < fNPar; i++)
        {
            printf("%s  Par %2d: %s (%s)\n",
                   option, i, fPar[i]->GetName(), fPar[i]->GetTitle());
            printf("%s          Value: %14.7e  Error: %14.7e\n",
                   option, fPar[i]->getVal(), fPar[i]->getError());
            printf("%s          Min  : %14.7e  Max  : %14.7e\n",
                   option, fPar[i]->getMin(), fPar[i]->getMax());
        }
    }
}

