
#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "globals.hh"

class G4GeneralParticleSource;
class G4Event;
class G4Box;

/// The primary generator action class with particle gun.
///
/// The default kinematic is a 14 MeV neutron, randomly distribued 
/// in front of the phantom across 80% of the (X,Y) phantom size.

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();    
    virtual ~PrimaryGeneratorAction();

    public:
    void SetEnergy(G4double ene) {particle_energy = ene;}
    G4double GetEnergy() {return particle_energy;}

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);         
  
    // method to access particle gun
    const G4GeneralParticleSource* GetGeneralParticleSource() const { return fParticleSource; }
  
  private:
    G4GeneralParticleSource*  fParticleSource; 
    G4Box* fEnvelopeBox;
    G4double particle_energy;
};


#endif

