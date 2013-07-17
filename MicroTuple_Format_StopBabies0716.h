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
#define MICROEVENT_FORMATROOT "event:triggerMu:triggerEl:leptonPt:leptonEta:leptonPhi:leptonE:leptonPFPt:leptonIso:leptonEpin:leptonPDG:nJets:nB:leadingBPt:MET:isoTrackVeto:tauVeto:MT:dPhiMETjet:hadronicChi2:MT2W:HTratio:dRleptonB:"



#endif


