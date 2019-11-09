/*************************************************************************
 * Author: Dominik Werthmueller, 2015-2019
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// LinkDef.h                                                            //
//                                                                      //
// FooFit dictionary header file.                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifdef __CINT__

// turn everything off
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link off all typedef;

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedef;

#pragma link C++ enum EFFMinimizer;
#pragma link C++ typedef FFMinimizer_t;

#pragma link C++ namespace FFFooFit;
#pragma link C++ class FFRooModel+;
#pragma link C++ class FFRooModelExpo+;
#pragma link C++ class FFRooModelGauss+;
#pragma link C++ class FFRooModelGaussBifur+;
#pragma link C++ class FFRooModelHist+;
#pragma link C++ class FFRooModelPol+;
#pragma link C++ class FFRooModelChebychev+;
#pragma link C++ class FFRooModelKeys+;
#pragma link C++ class FFRooModelComp+;
#pragma link C++ class FFRooModelProd+;
#pragma link C++ class FFRooModelSum+;
#pragma link C++ class FFRooFit+;
#pragma link C++ class FFRooFitHist+;
#pragma link C++ class FFRooFitTree+;
#pragma link C++ class FFRooSPlot+;
#pragma link C++ class FFRooFitter+;
#pragma link C++ class FFRooFitterUnbinned+;
#pragma link C++ class FFRooFitterBinned+;
#pragma link C++ class FFRooFitterSPlot+;
#pragma link C++ class FFRooFitterSpecies+;

#endif

