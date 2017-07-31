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
    fYieldInit = 0;
    fYieldFit = 0;
    fYieldFitError = 0;
    fYieldMin = 0;
    fYieldMax = 0;
}

//______________________________________________________________________________
FFRooFitterSpecies::FFRooFitterSpecies(const Char_t* name, const Char_t* title,
                                       FFRooModel* model)
    : TNamed(name, title)
{
    // Constructor.

    // init members
    fModel = model;
    fYieldInit = 0;
    fYieldFit = 0;
    fYieldFitError = 0;
    fYieldMin = 0;
    fYieldMax = 0;
}

//______________________________________________________________________________
FFRooFitterSpecies::~FFRooFitterSpecies()
{
    // Destructor.

    if (fModel) delete fModel;
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
void FFRooFitterSpecies::Print(Option_t* option) const
{
    // Print out the content of this class.

    printf("FFRooFitterSpecies content:\n");
    printf("Name                      : %s\n", GetName());
    printf("Title                     : %s\n", GetTitle());
    printf("Initial yield             : %e\n", fYieldInit);
    printf("Fitted yield              : %e\n", fYieldFit);
    printf("Fitted yield error        : %e\n", fYieldFitError);
    printf("Yield minimum             : %e\n", fYieldMin);
    printf("Yield maximum             : %e\n", fYieldMax);
}

