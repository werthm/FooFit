{
    gSystem->Load("libFooFit.so");

    TChain chain("events");
    chain.AddFile("data.root");

    gRandom->SetSeed(123);
    TF1* fIMSig = new TF1("fIMSig", "gaus", 0, 300);
    fIMSig->SetParameters(1, 135, 5);
    TH1* hPdf = new TH1F("hPdf", "hPdf", 150, 0, 300);
    for (Int_t i = 0; i < 1e5; i++) hPdf->Fill(fIMSig->GetRandom());

    FFRooModelHist model_sig("IM_Signal", "im signal", hPdf);

    FFRooModelExpo model_bg("IM_BG", "im background");
    model_bg.SetParameter(0, -0.04, -1, 1);

    FFRooModel* models[2] = { &model_sig, &model_bg };
    FFRooModelSum model_sum("IM_Total", "total fit", 2, models);
    model_sum.SetParameter(0, 1e5, 0, 1e6);
    model_sum.SetParameter(1, 1e5, 0, 1e6);

    FFRooFitTree fit(&chain, 1);
    fit.SetVariable(0, "im", "invariant mass", 0, 300);
    fit.SetModel(&model_sum);
    fit.Fit();
    fit.DrawFit();
}

