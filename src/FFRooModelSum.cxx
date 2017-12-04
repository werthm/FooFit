/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2017
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
    : FFRooModelComp(name, title, n, n, list)
{
    // Constructor.

    // add coefficient parameters
    for (Int_t i = 0; i < fNModel; i++)
    {
        TString tmp = TString::Format("%s_Coeff_%d", GetName(), i);
        AddParameter(i, tmp.Data(), tmp.Data());
    }
}

//______________________________________________________________________________
void FFRooModelSum::BuildModel(RooRealVar** vars)
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

    // prepare models to sum
    RooArgList modelList;
    RooArgList coeffList;
    for (Int_t i = 0; i < fNModel; i++)
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

//______________________________________________________________________________
void FFRooModelSum::Print(Option_t* option) const
{
    // Print out the content of this class.

    // call parent method
    FFRooModel::Print(option);

    printf("%sFFRooModelSum content:\n", option);
    if (fNModel)
    {
        printf("%sModel summands\n", option);
        for (Int_t i = 0; i < fNModel; i++)
        {
            printf("%s  Summand %2d:\n", option, i);
            fModelList[i]->Print(TString::Format("%s    ", option).Data());
        }
    }
}

