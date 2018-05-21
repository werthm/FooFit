{
    gSystem->Load("libFooFit.so");

    TChain chain("events");
    chain.AddFile("data.root");

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

    FFRooModelGauss model_sig_cop("Cop_Signal", "cop signal");
    model_sig_cop.SetParameter(0, 0, -10, 10);
    model_sig_cop.SetParameter(1, 5, 0, 12);

    FFRooModelGauss model_bg_cop("Cop_BG", "cop background");
    model_bg_cop.SetParameter(0, 0, -50, 50);
    model_bg_cop.SetParameter(1, 180, 150, 300);

    FFRooModel* models_sig[3] = { &model_sig_im, &model_sig_mm, &model_sig_cop };
    FFRooModelProd model_sig("Signal", "total signal", 3, models_sig);

    FFRooModel* models_bg[3] = { &model_bg_im, &model_bg_mm, &model_bg_cop };
    FFRooModelProd model_bg("BG", "total background", 3, models_bg);

    FFRooModel* models[2] = { &model_sig, &model_bg };
    FFRooModelSum model("Total", "total fit", 2, models);
    model.SetParameter(0, 1e5, 0, 1e6);
    model.SetParameter(1, 1e5, 0, 1e6);

    FFRooFitTree fit(&chain, 3, "FooFit_Fit", "a FooFit fit", 0, kTRUE);
    fit.SetVariable(0, "im", "invariant mass", 0, 300);
    fit.SetVariable(1, "mm", "missing mass", -500, 500);
    fit.SetVariable(2, "cop", "coplanarity", -180, 180);
    fit.SetModel(&model);
    fit.Fit();
    fit.DrawFit();
}

