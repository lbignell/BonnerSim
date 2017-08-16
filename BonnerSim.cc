#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#include "G4Threading.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"

#include "Randomize.hh"
#include "G4SDManager.hh"
#include <fstream>
#include "SensD.hh"
#include "G4VSensitiveDetector.hh"

#include "DetectorConstruction.hh"
#include "SABREPhysicsList.hh"
#include "ActionInitialization.hh"

#include "G4GenericBiasingPhysics.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

using namespace std;

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }
  
  // Evaluate arguments taken from 

  G4String onOffBiasing("");
  
  if ( onOffBiasing == "" ) onOffBiasing = "on";
  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
     
  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
  G4int ncores = G4Threading::G4GetNumberOfCores();
  runManager->SetNumberOfThreads(ncores-1);
#else
  G4RunManager* runManager = new G4RunManager;
#endif  

  // Set mandatory initialization classes
  //
  runManager->SetUserInitialization(new DetectorConstruction);

  //Create physics list
  SABREPhysicsList* physicsList = new SABREPhysicsList;
  // -- and augment it with biasing facilities:
  /*G4GenericBiasingPhysics* biasingPhysics = new G4GenericBiasingPhysics();
  if ( onOffBiasing == "on")
    {
      vector<G4String> proctobias;
      proctobias.push_back("protonInelastic");
      biasingPhysics->Bias("proton", proctobias);
      biasingPhysics->Bias("neutron");
      physicsList->RegisterPhysics(biasingPhysics);
      G4cout << "      ********************************************************* " << G4endl;
      G4cout << "      ********** processes are wrapped for biasing ************ " << G4endl;
      G4cout << "      ********************************************************* " << G4endl;
    }
  else
    {
      G4cout << "      ************************************************* " << G4endl;
      G4cout << "      ********** processes are not wrapped ************ " << G4endl;
      G4cout << "      ************************************************* " << G4endl;
    }*/
  
  runManager->SetUserInitialization(physicsList);
    
  // Set user action initialization
  //
  runManager->SetUserInitialization(new ActionInitialization());  
  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) { 
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else { 
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;
}

