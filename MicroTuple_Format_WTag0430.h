#ifndef microTupleFormat
#define microTupleFormat

// If you want to change the format being used by
// the proof jobs, you should edit the file
// ProofMicroTupler_Format.h


typedef struct 
{

    // Dataset
    Float_t trueNumberOfEvents;

    // Jets info
    Float_t nJets;
    Float_t nBtags;

    Float_t jet_pT[10];
    Float_t jet_eta[10];
    Float_t jet_phi[10];
    Float_t jet_bTag[10];

    // Lepton info
    Float_t lep_pT;
    Float_t lep_eta;
    Float_t lep_phi;
    Float_t lep_flavor;

    // Variables of intereset 
    Float_t MET;
    Float_t MT;
    Float_t HadronicChi2;
    Float_t MT2W;
    Float_t DPhi_MetJets;
    
    // Vetos
    Float_t isolatedTrackVeto;
    Float_t tauVeto;

    // Signal info
    Float_t mStop;
    Float_t mNeutralino;

    // W-tagging studies

        // Generated hadronic W
        Float_t genW_pT;
        Float_t genW_Eta;
        Float_t genW_Phi;

        // W-tagged jet candidates
        Float_t recoW_pT[10];
        Float_t recoW_Eta[10];
        Float_t recoW_Phi[10];
        Float_t recoW_Mass[10];


} microEvent;

//                       ############################################
//                       #            Keep me updated !             #
//                       ############################################
#define MICROEVENT_FORMATROOT "trueNumberOfEvents:nJets:nBtags:jet_pT[10]:jet_eta[10]:jet_phi[10]:jet_bTag[10]:lep_pT:lep_eta:lep_phi:lep_flavor:MET:MT:HadronicChi2:MT2W:DPhi_MetJets:isolatedTrackVeto:tauVeto:mStop:mNeutralino:genW_pT:genW_Eta:genW_Phi:recoW_pT[10]:recoW_Eta[10]:recoW_Phi[10]:recoW_Mass[10]"

/*
typedef struct
{

    Float_t mStop;
    Float_t mNeutralino;

} signalPoint;

#define SIGNALPOINT_FORMATROOT "mStop:mNeutralino"
*/
#endif





// ###################################################
// #     This function is called by the proof        #
// #   job - this is where you fill the microtuple   #
// ###################################################
#ifdef isCompilingTheMicroTupler
Bool_t MicroTuple_ProofJob::Process(Long64_t entry)
{

  // ########################################
  // #  Load the event from the input tree  #
  // ########################################
  
    fChain->GetTree()->GetEntry(entry); 
    branch->GetEntry(entry);
    IPHCTree::NTTransient::InitializeAfterReading(event);
    sel.LoadEvent(event);

  // #######################################################
  // #  Filter non-semileptonic events for signal dataset  #
  // #######################################################

    IPHCTree::NTMonteCarlo mcInfo = *((sel).GetPointer2MC());    

    
    if (dataset->Name() == "signal") 
    {
        int TMEME = mcInfo.TMEME; 
        int  MEME = TMEME % 10000; 
        int   EME =  MEME % 1000; 
        int    ME =   EME % 100; 
        int     E =    ME % 10;

        int nMCLepton = TMEME/ 10000 + ME/10 + E/1;

        if (nMCLepton != 1)  return true;
        else
        {
           stopMCinfo->LoadEvent(event);
           //mySignalPoint.mStop       = stopMCinfo->GetStopMass();
           //mySignalPoint.mNeutralino = stopMCinfo->GetNeutralinoMass();
           //theTree2->Fill();
        }
    }
    

  // ######################
  // #  Apply selection   #
  // ######################

    int triggerME = 0;
    int selection_lastStep = sel.doFullSelection(dataset,string("all"),&triggerME);

    // Reject events not passing selection until step 5
    if (selection_lastStep < 5) 
    {
        return true;
    }

    // #######################
    // #  Fill general info  #
    // #######################
    myEvent.trueNumberOfEvents = dataset->getNSkimmedEvent();

    // ####################
    // #  Fill jets info  #
    // ####################

    const std::vector<IPHCTree::NTJet> jetsForAna = sel.GetJetsForAna();
    myEvent.nJets  = jetsForAna.size();
    myEvent.nBtags = sel.GetBJetsForAna().size();
  
    for (int j = 0 ; j < jetsForAna.size() ; j++)
    {
       if (j >= 10) break;

       myEvent.jet_pT[j]   = jetsForAna[j].p4.Pt();
       myEvent.jet_eta[j]  = jetsForAna[j].p4.Eta();
       myEvent.jet_phi[j]  = jetsForAna[j].p4.Phi();

       if (jetsForAna[j].bTag["combinedSecondaryVertexBJetTags"] >= 0.679) 
           myEvent.jet_bTag[j] = 1.0;
       else
           myEvent.jet_bTag[j] = 0.0;
    }

    // ######################
    // #  Fill lepton info  #
    // ######################

    TLorentzVector lepton_p;
    int lepton_type;
    int lepton_charge;
    if (sel.GetMuonsForAna().size()==1)
    {
        lepton_p = (sel.GetMuonsForAna()[0]).p4;
        lepton_type = 2;
        lepton_charge = (sel.GetMuonsForAna()[0]).charge;
    }
    else
    {
        lepton_p = (sel.GetElectronsForAna()[0]).p4;
        lepton_type = 1;
        lepton_charge = (sel.GetElectronsForAna()[0]).charge;
    }
 
    myEvent.lep_pT     = lepton_p.Pt();
    myEvent.lep_eta    = lepton_p.Eta();
    myEvent.lep_phi    = lepton_p.Phi();
    myEvent.lep_flavor = lepton_type;   

    // #####################################
    // #  Fill event variable of interest  #
    // #####################################
  
    myEvent.MET          = sel.Met();
    myEvent.MT           = sel.MT_wleptonic();
    myEvent.HadronicChi2 = sel.HadronicChi2();
    myEvent.MT2W         = sel.MT2W();
    myEvent.DPhi_MetJets = sel.DPhi_MET_leadingJets();
    
    // ####################
    // #  Fill veto info  #
    // ####################
    
    myEvent.isolatedTrackVeto = 0.0;
    myEvent.tauVeto = 0.0;
    
    if (selection_lastStep >= 6)
        myEvent.isolatedTrackVeto = 1.0;
    if (selection_lastStep >= 7)
        myEvent.tauVeto = 1.0;
    
    // ######################
    // #  Fill signal info  #
    // ######################

    if (dataset->Name() == "signal")
    {
        stopMCinfo->LoadEvent(event);
        myEvent.mStop       = stopMCinfo->GetStopMass();
        myEvent.mNeutralino = stopMCinfo->GetNeutralinoMass();
    }
    else
    {
        myEvent.mStop       = -1;
        myEvent.mNeutralino = -1;
    }
    
    // #########################
    // #  Fill W-tagging info  #
    // #########################

    // Find hadronic W (assume there's only one)
     
        vector<IPHCTree::NTGenParticle> MCParticles = mcInfo.genParticles;
        int genW_Index = -1;
        for (unsigned int i = 0 ; i < MCParticles.size() ; i++)  
        {
            if (MCParticles[i].motherIndex_ == -1) continue;
            if ( (abs(MCParticles[MCParticles[i].motherIndex_].id) == 24)
            && (abs(MCParticles[i].id) <= 5) )
            {
                genW_Index = MCParticles[i].motherIndex_;
                break;
            }
        }

        if (genW_Index != -1)
        {
            myEvent.genW_pT  = MCParticles[genW_Index].p4.Pt();
            myEvent.genW_Eta = MCParticles[genW_Index].p4.Eta();
            myEvent.genW_Phi = MCParticles[genW_Index].p4.Phi();
        }
        else
        {
            myEvent.genW_pT  = -1;
            myEvent.genW_Eta = 999.0;
            myEvent.genW_Phi = 999.0;
        }

    // Loop on W-candidate collection

        std::vector<IPHCTree::NTJet> WCand = sel.GetHeavyTagJets();
        for (unsigned int i = 0 ; i < WCand.size() ; i++)
        {
            if (i >= 10) break;
            if (WCand[i].p4.Pt() < 20)
            {
                myEvent.recoW_pT[i] = 0;
                break;
            }

            myEvent.recoW_pT[i]   = WCand[i].p4.Pt();
            myEvent.recoW_Eta[i]  = WCand[i].p4.Eta();
            myEvent.recoW_Phi[i]  = WCand[i].p4.Phi();
            myEvent.recoW_Mass[i] = WCand[i].p4.M();
        }

    // ###############################
    // #  Add the event to the tree  #
    // ###############################
    
    theTree->Fill();

    return true;
}
#endif

