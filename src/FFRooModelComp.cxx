/*************************************************************************
 * Author: Dominik Werthmueller, 2017
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelComp                                                       //
//                                                                      //
// Abstract class representing a composite model for RooFit.            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooRealVar.h"

#include "FFRooModelComp.h"

ClassImp(FFRooModelComp)

//______________________________________________________________________________
FFRooModelComp::FFRooModelComp(const Char_t* name, const Char_t* title, Int_t nPar,
                               Int_t n, FFRooModel** list)
    : FFRooModel(name, title, nPar)
{
    // Constructor.

    // init members
    fNModel = n;
    fModelList = new FFRooModel*[fNModel];

    // loop over models
    for (Int_t i = 0; i < fNModel; i++)
    {
        // set model pointer
        if (list)
            fModelList[i] = list[i];
        else
            fModelList[i] = 0;
    }
}

//______________________________________________________________________________
FFRooModelComp::~FFRooModelComp()
{
    // Destructor.

    if (fModelList)
        delete [] fModelList;
}

//______________________________________________________________________________
void FFRooModelComp::SetModelList(FFRooModel** list)
{
    // Set all elements of the model list by setting pointers to the elements
    // of the array 'list'.

    // loop over model list length
    for (Int_t i = 0; i < fNModel; i++)
    {
        // set model pointer
        fModelList[i] = list[i];
    }
}

//______________________________________________________________________________
void FFRooModelComp::SetModel(Int_t i, FFRooModel* model)
{
    // Set the pointer to the model with index 'i' to 'model'.

    // check model index
    if (CheckModelBounds(i, "SetModel()"))
        fModelList[i] = model;
}

//______________________________________________________________________________
Bool_t FFRooModelComp::CheckModelBounds(Int_t mod, const Char_t* loc) const
{
    // Check if the model index 'mod' is within valid bounds.
    // Return kTRUE if the index 'mod' is valid, otherwise kFALSE.
    // Use 'loc' to set the location of the error.

    // check index range
    if (mod < 0 || mod >= fNModel)
    {
        Error("CheckModelBounds", "%s: Invalid model index %d (number of models: %d)",
              loc, mod, fNModel);
        return kFALSE;
    }
    else
    {
        return kTRUE;
    }
}

