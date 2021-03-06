#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4ParameterManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "Analysis.hh"

RunAction::RunAction()
 : G4UserRunAction(),
   fGoodEvents("GoodEvents", 0),
   fSumDose("SumDose", 0.)  
{
 // Create analysis manager 
  // The choice of analysis technology is done via selectin of a namespace 
  // in Analysis.hh 
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance(); 
  G4cout << "Using " << analysisManager->GetType() << G4endl; 
  
  // Default settings 
  analysisManager->SetVerboseLevel(1); 
  analysisManager->SetFileName("CLYC_data"); 
  
  // Creating ntuple here
  //
  //analysisManager->SetNtupleMerging(true);
  // first ntuple (id = 0) for CLYC sensitive detector
  analysisManager->CreateNtuple("CLYC", "Hits"); 
  analysisManager->CreateNtupleDColumn("edep"); // column Id = 0 
  analysisManager->CreateNtupleSColumn("procname"); // column Id = 1
  analysisManager->CreateNtupleDColumn("KinEn"); // column Id = 2 
  analysisManager->CreateNtupleDColumn("vertex_x"); // column Id = 3 
  analysisManager->CreateNtupleDColumn("vertex_y"); // column Id = 4 
  analysisManager->CreateNtupleDColumn("vertex_z"); // column Id = 5
  analysisManager->CreateNtupleSColumn("particle");//6
  analysisManager->CreateNtupleDColumn("vertex_px");//7
  analysisManager->CreateNtupleDColumn("vertex_py");//8
  analysisManager->CreateNtupleDColumn("vertex_pz");//9
  analysisManager->CreateNtupleDColumn("x");//10
  analysisManager->CreateNtupleDColumn("y");//11
  analysisManager->CreateNtupleDColumn("z");//12
  analysisManager->CreateNtupleDColumn("px");//13
  analysisManager->CreateNtupleDColumn("py");//14
  analysisManager->CreateNtupleDColumn("pz");//15
  analysisManager->CreateNtupleIColumn("evt");//16
  analysisManager->CreateNtupleDColumn("edep_evt");//17
  analysisManager->CreateNtupleDColumn("weight"); //18
  analysisManager->CreateNtupleIColumn("stepNum");//19
  analysisManager->CreateNtupleSColumn("creatorProc");//20 
  analysisManager->CreateNtupleIColumn("trackID");//21
  analysisManager->CreateNtupleDColumn("vertex_KinEn");//22
  analysisManager->FinishNtuple(); //Do not forget this line! 

}
RunAction::~RunAction()
{ }
void RunAction::BeginOfRunAction(const G4Run* run)
{ 
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
  // reset parameters to their initial values
  G4ParameterManager* parameterManager = G4ParameterManager::Instance();
  parameterManager->Reset();
  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
 
  // Get analysis manager 
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance(); 
  // Open an output file 
  // The default file name is set in RunAction::RunAction(), 
  // it can be overwritten in a macro 
  analysisManager->OpenFile();
}
void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;
  // Merge parameters 
  G4ParameterManager* parameterManager = G4ParameterManager::Instance();
  parameterManager->Merge();
  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const PrimaryGeneratorAction* generatorAction
    = static_cast<const PrimaryGeneratorAction*>(
        G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String partName;
  if (generatorAction) 
  {
    G4ParticleDefinition* particle 
      = generatorAction->GetGeneralParticleSource()->GetParticleDefinition();
    partName = particle->GetParticleName();
  }

  //get anlysis manager, write, then close file
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance(); 
  analysisManager->Write(); 
  analysisManager->CloseFile();   
}
