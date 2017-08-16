#include "globals.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4PhysicalVolumeStore.hh"

#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"

//---------------------------------------------------------------------------//

DetectorConstructionMessenger::DetectorConstructionMessenger
(DetectorConstruction *detector):fDetectorPrimary(detector)
{
    fDetMessDir = new G4UIdirectory("/DetectorMessenger");
    fDetMessDir->SetGuidance("UI commands to modify the geometry");

    fHDPECmd = new G4UIcmdWithADouble("/DetectorMessenger/setHDPEthickness",this);
    fHDPECmd->SetGuidance("Set the HDPE absorber thickness in Inches");
    fHDPECmd->SetParameterName("choice",false);
    fHDPECmd->AvailableForStates(G4State_Init,G4State_Idle);
    
    fupdateCmd = new G4UIcmdWithoutParameter("/DetectorMessenger/updateGeo", this);
    fupdateCmd->SetGuidance("Update detector geometry after settings");
}

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
    delete fDetMessDir;
    delete fHDPECmd;
    delete fupdateCmd;
}

//---------------------------------------------------------------------------//

void DetectorConstructionMessenger::SetNewValue(G4UIcommand *command,
                                                     G4String newValue)
{
    if (command == fHDPECmd )
    {
        fDetectorPrimary->SetHDPEthickness_in(fHDPECmd->GetNewDoubleValue(newValue));
    }
    else if ( command == fupdateCmd )
      {
    	fDetectorPrimary->UpdateGeometry();
      }
}
