{
    gSystem->Load("libFooFit.so");

    TChain chain("events");
    chain.AddFile("data.root");

    gRandom->SetSeed(123);
    TF1* fIMSig = new TF1("fIMSig", "gaus", 0, 300);
    fIMSig->SetParameters(1, 135, 5);
    TF1* fMMSig = new TF1("fMMSig", "gaus", -100, 100);
    fMMSig->SetParameters(1, 0, 20);
    TH2* hPdf = new TH2F("hPdf", "hPdf", 100, 0, 300, 100, -500, 500);
    for (Int_t i = 0; i < 1e5; i++) hPdf->Fill(fIMSig->GetRandom(), fMMSig->GetRandom());

    FFRooModelExpo model_bg_im("IM_BG", "im background");
    model_bg_im.SetParameter(0, -0.04, -1, 1);

    FFRooModelGauss model_bg_mm("MM_BG", "mm background");
    model_bg_mm.SetParameter(0, 150, 130, 170);
    model_bg_mm.SetParameter(1, 120, 100, 150);

    FFRooModelHist model_sig("Signal", "signal", hPdf);

    FFRooModel* models_bg[2] = { &model_bg_im, &model_bg_mm };
    FFRooModelProd model_bg("BG", "total background", 2, models_bg);

    FFRooModel* models[2] = { &model_sig, &model_bg };
    FFRooModelSum model("Total", "total fit", 2, models);
    model.SetParameter(0, 1e5, 0, 1e6);
    model.SetParameter(1, 1e5, 0, 1e6);

    FFRooFitTree fit(&chain, 2);
    fit.SetVariable(0, "im", "invariant mass", 0, 300);
    fit.SetVariable(1, "mm", "missing mass", -500, 500);
    fit.SetModel(&model);
    fit.Fit();
    fit.DrawFit();
}

