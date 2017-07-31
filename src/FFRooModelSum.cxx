/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelSum                                                        //
//                                                                      //
// Class representing a sum of models for RooFit.                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooAddPdf.h"
#include "RooRealVar.h"

#include "FFRooModelSum.h"

ClassImp(FFRooModelSum)

//______________________________________________________________________________
FFRooModelSum::FFRooModelSum(const Char_t* name, const Char_t* title, Int_t n, FFRooModel** list)
    : FFRooModel(name, title, n)
{
    // Constructor.

    Char_t tmp[256];

    // init members
    fModelList = new FFRooModel*[fNPar];

    // loop over models to sum
    for (Int_t i = 0; i < fNPar; i++)
    {
        // add the coefficient parameter
        sprintf(tmp, "%s_Coeff_%d", GetName(), i);
        AddParameter(i, tmp, tmp);

        // set model pointer
        if (list) fModelList[i] = list[i];
        else fModelList[i] = 0;
    }
}

//______________________________________________________________________________
FFRooModelSum::~FFRooModelSum()
{
    // Destructor.

    if (fModelList) delete [] fModelList;
}

//______________________________________________________________________________
void FFRooModelSum::SetModelList(FFRooModel** list)
{
    // Set all elements of the model list by setting pointers to the elements
    // of the array 'list'.

    // loop over model list length
    for (Int_t i = 0; i < fNPar; i++)
    {
        // set model pointer
        fModelList[i] = list[i];
    }
}

//______________________________________________________________________________
void FFRooModelSum::SetModel(Int_t i, FFRooModel* model)
{
    // Set the pointer to the model with index 'i' to 'model'.

    // check model index
    if (CheckModelBounds(i, "SetModel()"))
        fModelList[i] = model;
}

//______________________________________________________________________________
Bool_t FFRooModelSum::CheckModelBounds(Int_t mod, const Char_t* loc) const
{
    // Check if the model index 'mod' is within valid bounds.
    // Return kTRUE if the index 'mod' is valid, otherwise kFALSE.
    // Use 'loc' to set the location of the error.

    // check index range
    if (mod < 0 || mod >= fNPar)
    {
        Error("CheckModelBounds", "%s: Invalid model index %d (number of models: %d)",
              loc, mod, fNPar);
        return kFALSE;
    }
    else return kTRUE;
}

//______________________________________________________________________________
void FFRooModelSum::BuildModel(RooRealVar** vars)
{
    // Build the model using the variables 'vars'.

    // check model list elements
    for (Int_t i = 0; i < fNPar; i++)
    {
        if (!fModelList[i])
        {
            Error("BuildModel", "Model list element %d was not set!", i);
            return;
        }
    }

    // prepare models to sum
    RooArgList modelList;
    RooArgList coeffList;
    for (Int_t i = 0; i < fNPar; i++)
    {
        // build model and set fit variables
        fModelList[i]->BuildModel(vars);

        // add model pdf to argument list
        modelList.add(*fModelList[i]->GetPdf());

        // add coefficient parameter to argument list
        coeffList.add(*fPar[i]);
    }

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooAddPdf(GetName(), GetTitle(), modelList, coeffList);
}

