#ifndef _DETECTORCONSTRUCTIONMESSENGER_HH
#define _DETECTORCONSTRUCTIONMESSENGER_HH

//---------------------------------------------------------------------------//

#include "globals.hh"
#include "G4UImessenger.hh"

//---------------------------------------------------------------------------//
class G4UIdirectory;
class G4UIcmdWithADouble;
class G4UIcmdWithoutParameter;
class G4UIcmdWithABool;
class DetectorConstruction;
//---------------------------------------------------------------------------//

class DetectorConstructionMessenger : public G4UImessenger
{
public:
    
    //default constructor
    DetectorConstructionMessenger(DetectorConstruction *detector);
    
    //copy constructor
    //DetectorConstructionMessenger();
    
    //destructor
    ~DetectorConstructionMessenger();
    
    //public interface
    void SetNewValue(G4UIcommand *command, G4String newValues);
    
    //protected members
protected:
    
    //private  members
private:
    DetectorConstruction   *fDetectorPrimary;
    G4UIdirectory               *fDetMessDir;
    G4UIcmdWithADouble          *fHDPECmd;
    G4UIcmdWithoutParameter     *fupdateCmd;
    G4UIcmdWithABool		*fSphereCmd;
};
#endif
