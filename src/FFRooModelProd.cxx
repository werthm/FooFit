/*************************************************************************
 * Author: Dominik Werthmueller, 2015
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelProd                                                       //
//                                                                      //
// Class representing a product of models for RooFit.                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RooProdPdf.h"
#include "RooRealVar.h"

#include "FFRooModelProd.h"

ClassImp(FFRooModelProd)

//______________________________________________________________________________
FFRooModelProd::FFRooModelProd(const Char_t* name, const Char_t* title, Int_t n, FFRooModel** list)
    : FFRooModel(name, title, 0)
{
    // Constructor.

    // init members
    fNModel = n;
    fModelList = new FFRooModel*[fNModel];

    // loop over models to multiply
    for (Int_t i = 0; i < fNModel; i++)
    {
        // set model pointer
        if (list) fModelList[i] = list[i];
        else fModelList[i] = 0;
    }
}

//______________________________________________________________________________
FFRooModelProd::~FFRooModelProd()
{
    // Destructor.

    if (fModelList) delete [] fModelList;
}

//______________________________________________________________________________
void FFRooModelProd::SetModelList(FFRooModel** list)
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
void FFRooModelProd::BuildModel(RooRealVar** vars)
{
    // Build the model using the variables 'vars'.

    // check model list elements
    for (Int_t i = 0; i < fNModel; i++)
    {
        if (!fModelList[i])
        {
            Error("BuildModel", "Model list element %d was not set!", i);
            return;
        }
    }

    // prepare models to multiply
    RooArgList modelList;
    for (Int_t i = 0; i < fNModel; i++)
    {
        // build model and set fit variables
        fModelList[i]->BuildModel(vars+i);

        // add model pdf to argument list
        modelList.add(*fModelList[i]->GetPdf());
    }

    // create the model pdf
    if (fPdf) delete fPdf;
    fPdf = new RooProdPdf(GetName(), GetTitle(), modelList);
}

