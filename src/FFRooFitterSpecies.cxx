/*************************************************************************
 * Author: Dominik Werthmueller, 2017
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooFitterSpecies                                                   //
//                                                                      //
// Base class representing a species to be fit to data.                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "FFRooFitterSpecies.h"
#include "FFRooModel.h"

ClassImp(FFRooFitterSpecies)

//______________________________________________________________________________
FFRooFitterSpecies::FFRooFitterSpecies(const Char_t* name, const Char_t* title)
    : TNamed(name, title)
{
    // Constructor.

    // init members
    fModel = 0;
    fModelClass = "";
    fNModelPar = 0;
    fModelParName = 0;
    fModelParValue = 0;
    fModelParError = 0;
    fYieldInit = 0;
    fYieldFit = 0;
    fYieldFitError = 0;
    fYieldMin = 0;
    fYieldMax = 0;
    fYieldConstrGMean = 0;
    fYieldConstrGSigma = 0;
}

//______________________________________________________________________________
FFRooFitterSpecies::FFRooFitterSpecies(const Char_t* name, const Char_t* title,
                                       FFRooModel* model)
    : TNamed(name, title)
{
    // Constructor.

    // init members
    fModel = model;
    fModelClass = model->ClassName();
    fNModelPar = 0;
    fModelParName = 0;
    fModelParValue = 0;
    fModelParError = 0;
    fYieldInit = 0;
    fYieldFit = 0;
    fYieldFitError = 0;
    fYieldMin = 0;
    fYieldMax = 0;
    fYieldConstrGMean = 0;
    fYieldConstrGSigma = 0;
}

//______________________________________________________________________________
FFRooFitterSpecies::~FFRooFitterSpecies()
{
    // Destructor.

    if (fModel) delete fModel;
    if (fModelParName)
    {
        for (Int_t i = 0; i < fNModelPar; i++)
            delete fModelParName[i];
        delete [] fModelParName;
    }
    if (fModelParValue) delete [] fModelParValue;
    if (fModelParError) delete [] fModelParError;
}

//______________________________________________________________________________
void FFRooFitterSpecies::SetYield(Double_t init, Double_t min, Double_t max)
{
    // Configure the species yield fit parameter using the initial value 'init',
    // the mininum value 'min' and the maximum value 'max'.

    fYieldInit = init;
    fYieldMin = min;
    fYieldMax = max;
}

//______________________________________________________________________________
void FFRooFitterSpecies::UpdateModelParameters()
{
    // Update the model parameter members using the model member.

    // check for model
    if (fModel)
    {
        // delete old arrays
        if (fModelParName)
        {
            for (Int_t i = 0; i < fNModelPar; i++)
                delete fModelParName[i];
            delete [] fModelParName;
        }
        if (fModelParValue) delete fModelParValue;
        if (fModelParError) delete fModelParError;

        // update number of parameters
        fNModelPar = fModel->GetNPar();

        // create new arrays
        fModelParName = new TString*[fNModelPar];
        fModelParValue = new Double_t[fNModelPar];
        fModelParError = new Double_t[fNModelPar];

        // loop over parameters
        for (Int_t i = 0; i < fNModelPar; i++)
        {
            fModelParName[i] = new TString(fModel->GetParName(i));
            fModelParValue[i] = fModel->GetParameter(i);
            fModelParError[i] = fModel->GetParError(i);
        }
    }
}

//______________________________________________________________________________
void FFRooFitterSpecies::Print(Option_t* option) const
{
    // Print out the content of this class.

    printf("FFRooFitterSpecies content:\n");
    printf("Name                            : %s\n", GetName());
    printf("Title                           : %s\n", GetTitle());
    printf("Initial yield                   : %e\n", fYieldInit);
    printf("Fitted yield                    : %e\n", fYieldFit);
    printf("Fitted yield error              : %e\n", fYieldFitError);
    printf("Yield minimum                   : %e\n", fYieldMin);
    printf("Yield maximum                   : %e\n", fYieldMax);
    printf("Yield constraint Gaussian mean  : %e\n", fYieldConstrGMean);
    printf("Yield constraint Gaussian sigma : %e\n", fYieldConstrGSigma);
    printf("Model class name                : %s\n", fModelClass.Data());
    printf("Model parameters                : %d\n", fNModelPar);
    if (fNModelPar)
    {
        // loop over parameters
        for (Int_t i = 0; i < fNModelPar; i++)
        {
            printf("  %-30s: %14.7e +/- %14.7e\n",
                   fModelParName[i]->Data(), fModelParValue[i], fModelParError[i]);
        }
    }
}

