#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class SensD;
class SensDTarget;
class DetectorConstructionMessenger;
/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();               
    void SetHDPEthickness_in(G4double);
    void UpdateGeometry();
  private:
    void DefineMaterials();
    G4bool  fCheckOverlaps;
    G4String absoSDname;
    G4double HDPEthickness_in;
    DetectorConstructionMessenger* DetMess;
};

#endif

