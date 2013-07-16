
#define DEBUG_MSG cout << "DEBUG (" << __FILE__ << ", l." << __LINE__ << ") "


#include "MicroTuple_ProofJob.h"


#define isCompilingTheMicroTupler
#include "MicroTuple_ProofFormat.h"



//_____________________________________________________________________________
MicroTuple_ProofJob::MicroTuple_ProofJob()
{
  // Constructor
    
  fChain     = 0;
  branch     = 0;
  event      = 0;
  dataset    = 0;
  anaEL      = 0;
  verbosity  = 0;
  DataType   = 0;
  Luminosity = 0; 
  theTree    = 0;
  //theTree2   = 0;
  fFile      = 0;
  fProofFile = 0;
  stopMCinfo = new StopMCinfo();
}

//_____________________________________________________________________________
MicroTuple_ProofJob::~MicroTuple_ProofJob() {}

//_____________________________________________________________________________
void MicroTuple_ProofJob::Init(TTree *tree)
{
  cout << "Initializing the tree... " << endl;
  fChain = tree;

  // Find the relevant branch and link the NTEvent to it
  branch = (TBranch *) tree->GetBranch("NTEvent");
  event = new IPHCTree::NTEvent();
  branch->SetAddress(&event);

}

//_____________________________________________________________________________
void MicroTuple_ProofJob::Begin(TTree * /*tree*/)
{
}

//_____________________________________________________________________________
void MicroTuple_ProofJob::SlaveBegin(TTree * /*tree*/)
{

  cout << "      Starting microTupler job     " << endl;
  
  // ############################
  // #   Get input from PROOF   #
  // ############################

    // Dataset name
    TNamed *dsname = (TNamed *) fInput->FindObject("PROOF_DATASETNAME"); 
    datasetName = dsname->GetTitle();
  
    cout << "     > Dataset : " << datasetName << endl;

    // XML config
    TNamed *xfname = (TNamed *) fInput->FindObject("PROOF_XMLFILENAME");
    string xmlFileName = xfname->GetTitle();
  
    // Output file
    TNamed *out = (TNamed *) fInput->FindObject("PROOF_OUTPUTFILE");
    fProofFile = new TProofOutputFile(out->GetTitle());
    TDirectory *savedir = gDirectory;
    fFile = fProofFile->OpenFile("UPDATE");
    if (fFile && fFile->IsZombie()) SafeDelete(fFile);
    savedir->cd();

  // #######################
  // #   Load the config   #
  // #######################

    sel = Void;
    anaEL = new AnalysisEnvironmentLoader(xmlFileName.c_str());
    anaEL->LoadSamples(datasets, datasetName); 
    anaEL->LoadSelection(sel);  
    anaEL->LoadGeneralInfo(DataType, Luminosity, LumiError, verbosity);

    // Retrieve the current dataset according to its name
    for(unsigned int d=0;d<datasets.size();d++)
      if (datasets[d].Name() == datasetName) dataset = &datasets[d];

  // #############################
  // #   Initialise the TTree    #
  // #############################

    theTree=new TTree("microTuple","microTuple");
    theTree->SetDirectory(fFile);
    theTree->Branch("microEvents",&myEvent,MICROEVENT_FORMATROOT);
  
//    theTree2=new TTree("signalInitialNumberOfEvents","signalInitialNumberOfEvents");
//    theTree2->SetDirectory(fFile);
//    theTree2->Branch("signalPoint",&mySignalPoint,SIGNALPOINT_FORMATROOT);
}

//_____________________________________________________________________________
void MicroTuple_ProofJob::SlaveTerminate()
{

  if (fFile) 
  {
    fFile->cd();

    theTree->Print();
    theTree->Write(0, TObject::kOverwrite); 

    //theTree2->Print();
    //theTree2->Write(0, TObject::kOverwrite); 

    fProofFile->Print();
    fOutput->Add(fProofFile);

    fFile->Close("R");
  }

  delete anaEL;
  delete stopMCinfo;

  cout << endl;
  cout << "      Terminating microTupler job     " << endl;
  cout << endl;

}


//_____________________________________________________________________________
void MicroTuple_ProofJob::Terminate()
{
  sel=Void;
  datasets.clear();
  delete fFile;
  delete event;
  delete branch;
  delete fChain;
  delete dataset;
  delete gDirectory;
  delete fProofFile;
}
