#include "SABREPhysicsList.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4OpticalPhysics.hh" 
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmProcessOptions.hh"
#include "G4HadronPhysicsShielding.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4IonPhysics.hh"
#include "G4IonQMDPhysics.hh"
#include "G4IonElasticPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronElasticPhysicsLEND.hh"
#include "SABREStepMax.hh"
#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4DataQuestionaire.hh"

SABREPhysicsList::SABREPhysicsList(G4int verbose, G4String LEN_model, G4String HadrPhysVariant) 
  : G4VModularPhysicsList(){
  SetVerboseLevel(verbose);
  G4LossTableManager::Instance()->SetVerbose(verbose);  
  // Default Decay Physics
  RegisterPhysics(new G4DecayPhysics());
  // Default Radioactive Decay Physics
  RegisterPhysics(new G4RadioactiveDecayPhysics());
  RegisterPhysics(new G4EmStandardPhysics_option4());//recommended option
  size_t find = LEN_model.find("LEND__");
  G4String evaluation;
  if ( find != G4String::npos )
  {
      evaluation=LEN_model;
      evaluation.erase(0,find+6);
      LEN_model="LEND";
  }
  // Hadron Elastic scattering
  if ( LEN_model == "HP" ) 
  {
     RegisterPhysics( new G4HadronElasticPhysicsHP(verbose) );
  }
  else if ( LEN_model == "LEND" ) 
  {
     RegisterPhysics( new G4HadronElasticPhysicsLEND(verbose,evaluation) );
     G4DataQuestionaire itt(lend);
  }
  else 
  {
     G4cout << "Shielding Physics List: Warning!" <<G4endl;
     G4cout << "\"" << LEN_model << "\" is not valid for the low energy neutorn model." <<G4endl;
     G4cout << "Neutron HP package will be used." <<G4endl;
     RegisterPhysics( new G4HadronElasticPhysicsHP(verbose) );
  } 
  G4HadronPhysicsShielding* hps;
  // Hadron Physics
  if (HadrPhysVariant == "M") {
    hps = new G4HadronPhysicsShielding("hInelastic Shielding", verbose, 9.5*GeV, 9.9*GeV);
  } else {
    hps = new G4HadronPhysicsShielding("hInelastic Shielding", verbose, 4.0*GeV, 5.0*GeV);
  }
  if ( LEN_model == "HP" ) 
  {
     ;
  }
  else if ( LEN_model == "LEND" ) 
  {
     hps->UseLEND(evaluation); 
  }
  else 
  {}
  RegisterPhysics( hps );
  if ( LEN_model == "HP" ) {
     //Activate prodcuton of fission fragments in neutronHP
     char env_ff[]="G4NEUTRONHP_PRODUCE_FISSION_FRAGMENTS=1";
     putenv(env_ff);
  }
  RegisterPhysics( new G4StoppingPhysics(verbose) );
  RegisterPhysics( new G4IonQMDPhysics(verbose));  
  RegisterPhysics( new G4IonElasticPhysics(verbose));
  RegisterPhysics(new G4EmExtraPhysics());
  G4EmProcessOptions emOptions;
  emOptions.SetBuildCSDARange(true);//not really fundamental
  emOptions.SetFluo(true);
  emOptions.SetAuger(true);
  emOptions.SetPIXE(true);
  defaultCutValue = 1*mm;
  cutForGamma     = defaultCutValue;
  cutForElectron  = defaultCutValue;
  cutForPositron  = defaultCutValue;
}
SABREPhysicsList::~SABREPhysicsList()
{ 
}
void SABREPhysicsList::SetCuts()
{
  if (verboseLevel >0){
    G4cout << "PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");
  if (verboseLevel>0) DumpCutValuesTable();
}  
void SABREPhysicsList::AddStepMax()
{
  SABREStepMax* stepMaxProcess = new SABREStepMax();
  theParticleIterator->reset();
  while ((*theParticleIterator)()){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      if (stepMaxProcess->IsApplicable(*particle) && !particle->IsShortLived())
        {
	  pmanager ->AddDiscreteProcess(stepMaxProcess);
        }
  }
}
