{
    gSystem->Load("libFooFit.so");

    TChain chain("events");
    chain.AddFile("data.root");

    FFRooModelGauss model_sig("IM_Signal", "im signal");
    model_sig.SetParameter(0, 133, 128, 138);
    model_sig.SetParameter(1, 4, 2, 10);

    FFRooModelExpo model_bg("IM_BG", "im background");
    model_bg.SetParameter(0, -0.04, -1, 1);

    FFRooModel* models[2] = { &model_sig, &model_bg };
    FFRooModelSum model_sum("IM_Total", "total fit", 2, models);
    model_sum.SetParameter(0, 1e5, 0, 1e6);
    model_sum.SetParameter(1, 1e5, 0, 1e6);

    FFRooFitTree fit(&chain, 1, "FooFit_Fit", "a FooFit fit", 0, kTRUE);
    fit.SetVariable(0, "im", "invariant mass", 0, 300);
    fit.SetModel(&model_sum);
    fit.Fit();
    fit.DrawFit();
}

