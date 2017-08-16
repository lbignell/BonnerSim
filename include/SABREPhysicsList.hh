#ifndef SABREPhysicsList_h
#define SABREPhysicsList_h 1

#include "G4VModularPhysicsList.hh"

class SABREPhysicsList: public G4VModularPhysicsList
{
public:
  /// constructor
  SABREPhysicsList(G4int verbose = 1 , G4String low_energy_neutron_model = "HP", G4String HadrPhysVariant = "");
  /// destructor
  virtual ~SABREPhysicsList();
  //void ConstructParticle();
  //void ConstructProcess();    
  //void AddDecay();
  void AddStepMax();       

  /// Set user cuts
  virtual void SetCuts();

private:
  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;
  //SABREPhysicsListMessenger* pMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

