{
    gSystem->Load("libFooFit.so");

    TFile fin("data.root");
    TH1* histo = (TH1*) fin.Get("im_mm");

    FFRooModelGauss model_sig_im("IM_Signal", "im signal");
    model_sig_im.SetParameter(0, 133, 128, 138);
    model_sig_im.SetParameter(1, 4, 2, 10);

    FFRooModelExpo model_bg_im("IM_BG", "im background");
    model_bg_im.SetParameter(0, -0.04, -1, 1);

    FFRooModelGauss model_sig_mm("MM_Signal", "mm signal");
    model_sig_mm.SetParameter(0, 0, -10, 10);
    model_sig_mm.SetParameter(1, 20, 10, 30);

    FFRooModelGauss model_bg_mm("MM_BG", "mm background");
    model_bg_mm.SetParameter(0, 150, 130, 170);
    model_bg_mm.SetParameter(1, 120, 100, 150);

    FFRooModel* models_sig[2] = { &model_sig_im, &model_sig_mm };
    FFRooModelProd model_sig("Signal", "total signal", 2, models_sig);

    FFRooModel* models_bg[2] = { &model_bg_im, &model_bg_mm };
    FFRooModelProd model_bg("BG", "total background", 2, models_bg);

    FFRooModel* models[2] = { &model_sig, &model_bg };
    FFRooModelSum model("Total", "total fit", 2, models);
    model.SetParameter(0, 1e5, 0, 1e6);
    model.SetParameter(1, 1e5, 0, 1e6);

    FFRooFitHist fit(histo, 2);
    fit.SetVariable(0, "im", "invariant mass", 0, 300);
    fit.SetVariable(1, "mm", "missing mass", -500, 500);
    fit.SetModel(&model);
    fit.Fit();
    fit.DrawFit();
}

