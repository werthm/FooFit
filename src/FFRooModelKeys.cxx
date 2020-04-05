/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FFRooModelKeys                                                       //
//                                                                      //
// Class representing a model for RooFit using the n-dimensional kernel //
// estimation pdf of RooNDKeysPdf.                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TTree.h"
#include "RooDataSet.h"
#include "RooNDKeysPdf.h"
#include "RooRealVar.h"

#include "FFRooModelKeys.h"

ClassImp(FFRooModelKeys)

//______________________________________________________________________________
FFRooModelKeys::FFRooModelKeys(const Char_t* name, const Char_t* title, Int_t nDim, TTree* tree,
                               const Char_t* opt, Double_t rho, Int_t nSigma, Bool_t rotate)
    : FFRooModel(name, title, 0)
{
    // Constructor for 'nDim'-dim. pdf using RooNDKeysPdf.
    // See RooNDKeysPdf for meaning of parameters.

    // init members
    fType = kRooNDKeys;
    fNDim = nDim;
    fTree = tree;
    fDataSet = 0;
    fOpt = opt;
    fRho = rho;
    fNSigma = nSigma;
    fRotate = rotate;
    fMirror = RooKeysPdf::NoMirror;
}

//______________________________________________________________________________
FFRooModelKeys::FFRooModelKeys(const Char_t* name, const Char_t* title, TTree* tree, Bool_t addShiftPar,
                               RooKeysPdf::Mirror mirror, Double_t rho)
    : FFRooModel(name, title, addShiftPar ? 1 : 0)
{
    // Constructor for 1-dim. pdf using RooKeysPdf.
    // See RooKeysPdf for meaning of parameters.

    fType = kRooKeys;
    fNDim = 1;
    fTree = tree;
    fDataSet = 0;
    fOpt = "";
    fRho = rho;
    fNSigma = 0;
    fRotate = kFALSE;
    fMirror = mirror;

    // add shift parameters
    if (addShiftPar)
    {
        TString tmp = TString::Format("%s_Shift", GetName());
        AddParameter(0, tmp.Data(), tmp.Data());
    }
}

//______________________________________________________________________________
FFRooModelKeys::~FFRooModelKeys()
{
    // Destructor.

    if (fTree)
        delete fTree;
    if (fDataSet)
        delete fDataSet;
}

//______________________________________________________________________________
void FFRooModelKeys::BuildModel(RooAbsReal** vars)
{
    // Build the model using the variables 'vars'.

    // create argument set and list of variables
    RooArgSet varSet;
    RooArgList varList;
    for (Int_t i = 0; i < fNDim; i++)
    {
        varSet.add(*vars[i]);
        varList.add(*vars[i]);
    }

    // create RooFit dataset
    if (fDataSet)
        delete fDataSet;

    // extend range if shift parameter is present
    if (fNPar)
    {
        // get variable/parameter info
        RooRealVar* r = (RooRealVar*)vars[0];
        RooRealVar* p = (RooRealVar*)fPar[0];
        Double_t min = r->getMin();
        Double_t max = r->getMax();

        // adjust variable range
        r->setMin(TMath::Min(min, TMath::Min(min - p->getMin(), min - p->getMax())));
        r->setMax(TMath::Max(max, TMath::Max(max - p->getMin(), max - p->getMax())));

        // create data set
        fDataSet = new RooDataSet(fTree->GetName(), fTree->GetTitle(), varSet, RooFit::Import(*fTree));

        // restore variable range
        r->setMin(min);
        r->setMax(max);
    }
    else
    {
        fDataSet = new RooDataSet(fTree->GetName(), fTree->GetTitle(), varSet, RooFit::Import(*fTree));
    }

    // user info
    Int_t nEntries = fDataSet->numEntries();
    Info("BuildModel", "Entries in data tree      : %.9e", (Double_t)fTree->GetEntries());
    Info("BuildModel", "Entries in RooFit dataset : %.9e", (Double_t)nEntries);

    // create the model pdf
    if (fPdf)
        delete fPdf;

    // select pdf implementation
    if (fType == kRooNDKeys)
    {
        fPdf = new RooNDKeysPdf(GetName(), GetTitle(), varList, *fDataSet, fOpt.Data(), fRho, fNSigma, fRotate);
    }
    else if (fType == kRooKeys)
    {
        // check for shift parameter
        if (fNPar)
        {
            // add shift transformation
            TString tmp = TString::Format("%s - %s", vars[0]->GetName(), GetPar(0)->GetName());
            RooAbsReal* trans = new RooFormulaVar(TString::Format("Trans_Shift_%s_Var_%s", GetName(), vars[0]->GetName()).Data(),
                                                  tmp.Data(), RooArgSet(*vars[0], *GetPar(0)));
            AddVarTrans(trans);
            fPdf = new RooKeysPdf(GetName(), GetTitle(), *trans, *((RooRealVar*)(vars[0])), *fDataSet, fMirror, fRho);
        }
        else
        {
            fPdf = new RooKeysPdf(GetName(), GetTitle(), *vars[0], *fDataSet, fMirror, fRho);
        }
    }
    else
    {
        fPdf = 0;
        Error("BuildModel", "Unknown keys pdf type!");
    }
}

