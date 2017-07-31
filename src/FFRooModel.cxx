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


#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include "FFRooModel.h"

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
    for (Int_t i = 0; i < fNPar; i++) fPar[i] = 0;
    fNVarTrans = 0;
    fVarTrans = 0;
}

//______________________________________________________________________________
FFRooModel::~FFRooModel()
{
    // Destructor.

    if (fPdf) delete fPdf;
    if (fPar)
    {
        for (Int_t i = 0; i < fNPar; i++)
            if (fPar[i]) delete fPar[i];
        delete [] fPar;
    }
    if (fVarTrans)
    {
        for (Int_t i = 0; i < fNVarTrans; i++)
            if (fVarTrans[i]) delete fVarTrans[i];
        delete [] fVarTrans;
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
        if (fPar[i]) delete fPar[i];
        fPar[i] = new RooRealVar(name, title, -RooNumber::infinity(), RooNumber::infinity());
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
            fPar[i]->setVal(v);
            fPar[i]->setRange(min, max);
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
        if (fPar[i]) fPar[i]->setRange(min, max);
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

