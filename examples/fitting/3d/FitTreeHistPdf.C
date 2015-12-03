{
    gSystem->Load("libFooFit.so");

    TChain chain("events");
    chain.AddFile("data.root");

    gRandom->SetSeed(123);
    TF1* fIMSig = new TF1("fIMSig", "gaus", 0, 300);
    fIMSig->SetParameters(1, 135, 5);
    TF1* fMMSig = new TF1("fMMSig", "gaus", -100, 100);
    fMMSig->SetParameters(1, 0, 20);
    TF1* fCopSig = new TF1("fCopSig", "gaus", -180, 180);
    fCopSig->SetParameters(1, 0, 7);
    TH3* hPdf = new TH3F("hPdf", "hPdf", 100, 0, 300, 100, -500, 500, 100, -180, 180);
    for (Int_t i = 0; i < 1e6; i++) hPdf->Fill(fIMSig->GetRandom(), fMMSig->GetRandom(), fCopSig->GetRandom());

    FFRooModelExpo model_bg_im("IM_BG", "im background");
    model_bg_im.SetParameter(0, -0.04, -1, 1);

    FFRooModelGauss model_bg_mm("MM_BG", "mm background");
    model_bg_mm.SetParameter(0, 150, 130, 170);
    model_bg_mm.SetParameter(1, 120, 100, 150);

    FFRooModelGauss model_bg_cop("Cop_BG", "cop background");
    model_bg_cop.SetParameter(0, 0, -50, 50);
    model_bg_cop.SetParameter(1, 180, 150, 300);

    FFRooModelHist model_sig("Signal", "signal", hPdf);

    FFRooModel* models_bg[3] = { &model_bg_im, &model_bg_mm, &model_bg_cop };
    FFRooModelProd model_bg("BG", "total background", 3, models_bg);

    FFRooModel* models[2] = { &model_sig, &model_bg };
    FFRooModelSum model("Total", "total fit", 2, models);
    model.SetParameter(0, 1e5, 0, 1e6);
    model.SetParameter(1, 1e5, 0, 1e6);

    FFRooFitTree fit(&chain, 3);
    fit.SetVariable(0, "im", "invariant mass", 0, 300);
    fit.SetVariable(1, "mm", "missing mass", -500, 500);
    fit.SetVariable(2, "cop", "coplanarity", -180, 180);
    fit.SetModel(&model);
    fit.Fit();
    fit.DrawFit();
}

