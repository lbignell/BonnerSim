#ifndef SensD_h
#define SensD_h 1

#include "G4VSensitiveDetector.hh"
#include "DetectorConstruction.hh"
#include "SensDhit.hh"

#include <vector>
#include <string>

using namespace std;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class SensD : public G4VSensitiveDetector
{
  public:
    SensD(const G4String& name);
    virtual ~SensD();
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);
    G4int fHCID;
    G4String SDname;

    SensDhitCollection* fHitsCollection;
  private:
};

#endif

