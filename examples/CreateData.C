{
    Double_t event_id;
    Double_t im;
    Double_t mm;
    Double_t cop;

    TTree* tree = new TTree("events", "events");
    tree->Branch("event_id", &event_id);
    tree->Branch("im", &im);
    tree->Branch("mm", &mm);
    tree->Branch("cop", &cop);

    TH1* histo_im = new TH1F("im", "im", 250, 0, 500);
    TH1* histo_mm = new TH1F("mm", "mm", 500, -500, 500);
    TH2* histo_im_mm = new TH2F("im_mm", "im_mm", 250, 0, 500, 500, -500, 500);
    TH3* histo_im_mm_cop = new TH3F("im_mm_cop", "im_mm_cop", 250, 0, 500, 500, -500, 500, 90, -180, 180);

    TF1* fIMSig = new TF1("fIMSig", "gaus", 0, 200);
    fIMSig->SetParameters(1, 135, 5);
    TF1* fIMBG = new TF1("fIMBG", "expo", 0, 500);
    fIMBG->SetParameters(1, -0.01);

    TF1* fMMSig = new TF1("fMMSig", "gaus", -100, 100);
    fMMSig->SetParameters(1, 0, 20);
    TF1* fMMBG = new TF1("fMMBG", "gaus", -200, 500);
    fMMBG->SetParameters(1, 150, 120);

    TF1* fCopSig = new TF1("fCopSig", "gaus", -180, 180);
    fCopSig->SetParameters(1, 0, 7);
    TF1* fCopBG = new TF1("fCopBG", "gaus", -180, 180);
    fCopBG->SetParameters(1, 0, 250);

    gRandom->SetSeed(123);

    event_id = 1;
    for (Long64_t i = 0; i < 1e5; i++)
    {
        if (gRandom->Rndm() > 0.9)
        {
            im = fIMSig->GetRandom();
            mm = fMMSig->GetRandom();
            cop = fCopSig->GetRandom();
        }
        else
        {
            im = fIMBG->GetRandom();
            mm = fMMBG->GetRandom();
            cop = fCopBG->GetRandom();
        }

        tree->Fill();
        histo_im->Fill(im);
        histo_mm->Fill(mm);
        histo_im_mm->Fill(im, mm);
        histo_im_mm_cop->Fill(im, mm, cop);

        event_id++;
    }

    TFile* fout = new TFile("data.root", "recreate");
    tree->Write();
    histo_im->Write();
    histo_mm->Write();
    histo_im_mm->Write();
    histo_im_mm_cop->Write();
    delete fout;

    gSystem->Exit(0);
}

