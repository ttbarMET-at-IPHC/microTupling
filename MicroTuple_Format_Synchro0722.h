#ifndef microTupleFormat
#define microTupleFormat

// If you want to change the format being used by
// the proof jobs, you should edit the file
// ProofMicroTupler_Format.h


typedef struct 
{

    // Event info
    Float_t event;        

    // Trigger
    Float_t triggerMu;    
    Float_t triggerEl;    
    
    // Lepton info
    Float_t leptonPt;     
    Float_t leptonEta;    
    Float_t leptonPhi;    
    Float_t leptonE;      
    Float_t leptonPFPt;   
    Float_t leptonIso;    
    Float_t leptonEpin;   
    Float_t leptonPDG;    

    // Jet MET
    Float_t nJets;        
    Float_t nB;
    Float_t jets_Pt[10];
    Float_t jets_Phi[10];
    Float_t jets_Eta[10];
    Float_t leadingBPt;
    Float_t MET;

    // Vetos
    Float_t isoTrackVeto; 
    Float_t tauVeto;      

    // "High-level variables"
    Float_t MT;           
    Float_t dPhiMETjet;   
    Float_t hadronicChi2; 
    Float_t MT2W;         
    Float_t HTratio;      
    Float_t dRleptonB;

} microEvent;

//                       ############################################
//                       #            Keep me updated !             #
//                       ############################################
#define MICROEVENT_FORMATROOT "event:triggerMu:triggerEl:leptonPt:leptonEta:leptonPhi:leptonE:leptonPFPt:leptonIso:leptonEpin:leptonPDG:nJets:nB:jets_Pt[10]:jets_Phi[10]:jets_Eta[10]:leadingBPt:MET:isoTrackVeto:tauVeto:MT:dPhiMETjet:hadronicChi2:MT2W:HTratio:dRleptonB"


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

  // ######################
  // #  Apply selection   #
  // ######################

    int triggerME = 0;
    int selection_lastStep = sel.doFullSelection(dataset,string("all"),&triggerME);
       
    if (selection_lastStep < 3) return false; 

    myEvent.triggerEl = 0.0; if ((triggerME == 1 ) || (triggerME == 11)) myEvent.triggerEl = 1.0;
    myEvent.triggerMu = 0.0; if ((triggerME == 10) || (triggerME == 11)) myEvent.triggerMu = 1.0;

    // #######################
    // #  Fill general info  #
    // #######################

    myEvent.event = event->general.eventNb;
    
    // ####################
    // #  Fill jets info  #
    // ####################

    const std::vector<IPHCTree::NTJet> jetsForAna = sel.GetJetsForAna();
    myEvent.nJets      = jetsForAna.size();
    const std::vector<IPHCTree::NTJet> jets = *(sel.GetPointer2Jets());

    for (int i = 0 ; i < 10 ; i++)
    {
        if (i >= jets.size())
        {
            myEvent.jets_Pt[i]  = -1;
            myEvent.jets_Phi[i] = -999;
            myEvent.jets_Eta[i] = -999;
        }
        else
        {
            myEvent.jets_Pt[i]  = jets[i].p4.Pt();
            myEvent.jets_Phi[i] = jets[i].p4.Phi();
            myEvent.jets_Eta[i] = jets[i].p4.Eta();
        }
    }
   
    
    
    
    myEvent.nB         = sel.GetBJetsForAna().size();

    float leadingBPt = 0;
    for (int i = 0 ; i < sel.GetBJetsForAna().size() ; i++)
    {
        if (sel.GetBJetsForAna()[i].p4.Pt() > leadingBPt)
            leadingBPt = sel.GetBJetsForAna()[i].p4.Pt();
    }

    myEvent.leadingBPt = leadingBPt;

    // ######################
    // #  Fill lepton info  #
    // ######################

    TLorentzVector lepton_p;
    int lepton_type;
    float lepton_iso;
    float lepton_PFPt;
    float lepton_Epin;

    for (int i = 0 ; i < sel.GetMuonsForAna().size() ; i++)
    {
        if (lepton_p.Pt() > (sel.GetMuonsForAna()[i]).p4.Pt()) continue;
        
        lepton_p = (sel.GetMuonsForAna()[i]).p4;
        lepton_type = 13;

        float pfIsoCharged = (sel.GetMuonsForAna()[i]).isolation["PF03Char"];
        float pfIsoNeutral = (sel.GetMuonsForAna()[i]).isolation["PF03Neut"];
        float pfIsoPhoton  = (sel.GetMuonsForAna()[i]).isolation["PF03Phot"];
        float pfIsoPU      = (sel.GetMuonsForAna()[i]).isolation["PF03PU"];
        lepton_iso = pfIsoCharged + max(0., pfIsoNeutral + pfIsoPhoton- 0.5*pfIsoPU); 

        lepton_PFPt = (sel.GetMuonsForAna()[i]).bestMatch_pT;
        lepton_Epin = -9999.0;
    }
    for (int i = 0 ; i < sel.GetElectronsForAna().size() ; i++)
    {
        if (lepton_p.Pt() > (sel.GetElectronsForAna()[i]).p4.Pt()) continue;
        
        lepton_p = (sel.GetElectronsForAna()[i]).p4;
        lepton_type = 11;
        
        float chargedIso  = (sel.GetElectronsForAna()[i]).isolation["RA4Charg"];
        float photonIso   = (sel.GetElectronsForAna()[i]).isolation["RA4Photo"];
        float neutralIso  = (sel.GetElectronsForAna()[i]).isolation["RA4Neutr"];
        float rho_relIso  = (sel.GetElectronsForAna()[i]).isolation["rho"];
        float Aeff_relIso = (sel.GetElectronsForAna()[i]).isolation["Aeff"];
        lepton_iso = chargedIso + max((float) 0.0,(float) (photonIso + neutralIso - rho_relIso * Aeff_relIso));
        
        lepton_PFPt = (sel.GetElectronsForAna()[i]).bestMatch_pT;
	    lepton_Epin = (sel.GetElectronsForAna()[i]).eSuperClusterOverP;
    }    

    myEvent.leptonPt     = lepton_p.Pt();
    myEvent.leptonEta    = lepton_p.Eta();
    myEvent.leptonPhi    = lepton_p.Phi();
    myEvent.leptonE      = lepton_p.E();
    myEvent.leptonPDG    = lepton_type;
    myEvent.leptonIso    = lepton_iso;
    myEvent.leptonPFPt   = lepton_PFPt;
    
    // #####################################
    // #  Fill event variable of interest  #
    // #####################################
  
    myEvent.MET          = sel.Met();
    myEvent.hadronicChi2 = sel.HadronicChi2();
    myEvent.dPhiMETjet   = sel.DPhi_MET_leadingJets(); 
    myEvent.HTratio      = sel.HT_ratio();
 
    myEvent.MT2W = sel.MT2W();
    myEvent.MT = sel.MT_wleptonic();
       
    // ####################
    // #  Fill veto info  #
    // ####################
    
    myEvent.isoTrackVeto = 0.0;
    myEvent.tauVeto = 0.0;
    
    if (selection_lastStep >= 6)
        myEvent.isoTrackVeto = 1.0;
    if (selection_lastStep >= 7)
        myEvent.tauVeto = 1.0;
    
    // ###############################
    // #  Add the event to the tree  #
    // ###############################
    
    theTree->Fill();

    return true;
}
#endif

