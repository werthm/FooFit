{
    gSystem->Load("libFooFit.so");

    TChain chain("events");
    chain.AddFile("data.root");

    // fit

    FFRooModelGauss model_sig("IM_Signal", "im signal");
    model_sig.SetParameter(0, 133, 128, 138);
    model_sig.SetParameter(1, 4, 2, 10);

    FFRooModelExpo model_bg("IM_BG", "im background");
    model_bg.SetParameter(0, -0.04, -1, 1);

    FFRooSPlot splot(&chain, 1, 2);
    splot.SetVariable(0, "im", "invariant mass", 0, 500);
    splot.SetSpeciesName(0, "signal");
    splot.SetSpeciesName(1, "background");
    splot.SetSpeciesModel(0, &model_sig);
    splot.SetSpeciesModel(1, &model_bg);
    splot.SetSpeciesYield(0, 1e5, 0, 1e6);
    splot.SetSpeciesYield(1, 1e5, 0, 1e6);
    splot.Fit();
    splot.DrawFit();

    // weight

    Double_t im;
    Double_t mm;
    chain.SetBranchAddress("im", &im);
    chain.SetBranchAddress("mm", &mm);

    TH1* histo_mm = new TH1F("mm", "all events", 250, -500, 500);
    TH1* histo_mm_sig = new TH1F("mm_sig", "signal", 250, -500, 500);
    TH1* histo_mm_bg = new TH1F("mm_bg", "background", 250, -500, 500);

    for (Long64_t event = 0; event < chain.GetEntries(); event++)
    {
        chain.GetEntry(event);

        Double_t w_sig = splot.GetSpeciesWeight(event, 0);
        Double_t w_bg = splot.GetSpeciesWeight(event, 1);

        histo_mm->Fill(mm);
        histo_mm_sig->Fill(mm, w_sig);
        histo_mm_bg->Fill(mm, w_bg);
    }

    TCanvas* c = new TCanvas("Weighted", "Weighted", 700, 700);
    c->Divide(2, 2 );
    c->cd(1);
    histo_mm->Draw();
    c->cd(2);
    histo_mm_sig->Draw();
    c->cd(3);
    histo_mm_bg->Draw();
}

