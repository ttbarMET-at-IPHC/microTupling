//Modifie par Thomas DESCHLER

#include <TApplication.h>
#include <TGClient.h>
#include <TProof.h>
#include <TEnv.h>
#include <TChain.h>
#include <TFileCollection.h>
#include <TDrawFeedback.h>

#include "Tools/interface/Dataset.h"
#include "Tools/interface/AnalysisEnvironmentLoader.h"

#include "MicroTuple_ProofLauncher.h"
#include "EventReco/interface/Mt2Com_bisect.h"

int main(int argc, char* argv[])
{
  
  // ##########################
  // #   Setting parameters   #
  // ##########################
  
  int nwnodes = NUMBER_OF_NODES;

  string macroName = MACRO_NAME;
  string xmlFileName = getenv( "CMSSW_BASE" )+string("/src/NTuple/NTupleAnalysis/")+XML_CONFIG;
  string outputBox = string(OUTPUT_BOX);

  gEnv->SetValue("ProofLite.Sandbox",outputBox.c_str());
  string outputFileName = OUTPUT_NAME;
  
  // ###########################
  // #   Creating the TPROOF   #
  // ###########################
 
  // Cleaning proof stuff
  system("../GeneralExamples/./clean_proof.sh ; echo 'Cleaning proof stuff ... ' ; sleep 6");
  system((string("rm -r ")+OUTPUT_BOX).c_str());  

  TProof *proof = TProof::Open("");
  proof->SetParallel(nwnodes);

  // ########################
  // #   Loading packages   #
  // ########################
 
  cout<<"   > Loading NTuple Analysis package (don't worry about the symlink error)";
  proof->UploadPackage("../NTAna.par");
  proof->EnablePackage("NTAna");
  
  // ##############################
  // #   Load XML configuration   #
  // ##############################
  
  vector<Dataset> datasets;
  AnalysisEnvironmentLoader anaEL(xmlFileName);
  anaEL.LoadSamples(datasets);
 
  /*
      cout<<" #------------------------------------# "<<endl;
      cout<<" PROOF DATASETS SUMMARY [normaly 0]"<<endl;
      proof->ShowDataSets();
      cout<<" #------------------------------------# "<<endl;
      cout<<" # Registring dataset ... "<<endl;
      cout<<" Don't be worry with the checksum error message [at least I'm not ;-) ]"<<endl;
      cout<<" #------------------------------------# "<<endl;
  */

  //Create datasets in proof format
  TFileCollection** fileCollec = new TFileCollection*[datasets.size()];
  for(unsigned int i=0;i<datasets.size();i++)
  {
    fileCollec[i]  = new TFileCollection(datasets[i].Name().c_str(),"");
    for(unsigned int j=0;j<datasets[i].Filenames().size();j++)
    {
      fileCollec[i]->Add(datasets[i].Filenames()[j].c_str());
    }
    //register dataset in proof
    proof->RegisterDataSet(datasets[i].Name().c_str(),fileCollec[i]);
    proof->VerifyDataSet(datasets[i].Name().c_str());
  }
  
  /*
      //summarize the list of datasets
      cout<<" #------------------------------------# "<<endl;
      cout<<" PROOF DATASETS SUMMARY"<<endl;
      proof->ShowDataSets();
      cout<<" #------------------------------------# "<<endl;
  */ 

  // ############################
  // #   Process the datasets   #
  // ############################
  
  string outputFileNameModif = outputFileName.substr(0,outputFileName.size()-5);
  string MergingCommand = "hadd "+outputFileNameModif+"_merged.root ";
  
    //cout<<"datasets.size()= "<<datasets.size()<<endl;

   for(unsigned int i=0;i<datasets.size();i++)
   {
    cout<<"#######################################################"<<endl;
    cout<<"#   Processing the dataset "<<datasets[i].Name()<<"   #"<<endl;
    cout<<"#######################################################"<<endl;

    proof->AddInput(new TNamed("PROOF_DATASETNAME", datasets[i].Name()));
    proof->AddInput(new TNamed("PROOF_XMLFILENAME", xmlFileName));
    proof->AddInput(new TNamed("PROOF_OUTPUTFILE", outputFileName));
    
    /*
        cout<<"#------------------------------------# "<<endl;
        cout<<"PROOF PARAMETERS SUMMARY"<<endl;
        proof->ShowParameters();
        cout<<"#------------------------------------# "<<endl;
    */
    
    proof->Process(datasets[i].Name().c_str(),macroName.c_str(),"",datasets[i].NofEvtsToRunOver());
    system("sleep 5");
    
    string newFileName = outputFileNameModif+"_"+datasets[i].Name()+".root";
    cout<<"Copying the output file with the name "<<endl;
    string command = "cp "+outputFileName+" "+newFileName;
    MergingCommand+=newFileName+" ";
    system(command.c_str());
    proof->ClearInput();
  }
 
  // #########################
  // #   Merge the outputs   #
  // #########################
  
  system(MergingCommand.c_str());
  system("mkdir backup_outputProof`date +\"%d-%m-%y_%H-%M-%S\"`; mv proof*.root  backup_outputProof`date +\"%d-%m-%y_%H-%M-%S\"`/.");
  
  cout << "#################################################" << endl;
    proof->Print();
  cout << "#################################################" << endl;
    
  cout << "#################################################" << endl;
  cout << "#        May your job live long and prosper     #" << endl;
  cout << "#################################################" << endl;
  cout << "                                                 " << endl;
  cout << "                   _                             " << endl;
  cout << "                .-T |   _                        " << endl;
  cout << "                | | |  / |                       " << endl;
  cout << "                | | | / /`|                      " << endl;
  cout << "                 _  | | |/ / /                   " << endl;
  cout << "                 \\`\\| \'.\' / /                " << endl;
  cout << "              \\ \\`-. \'--|                     " << endl;
  cout << "               \\    \'   |                      " << endl;
  cout << "                \\ \\  .` /                      " << endl;
  cout << "                  |     |                        " << endl;
  cout << "                                                 " << endl;
  cout << "                                                 " << endl;
  cout << "#################################################"<<endl;
  
  return (0);
}
