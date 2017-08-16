#include "PrimaryGeneratorAction.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "G4SingleParticleSource.hh"
#include "G4ChargedGeantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleSource(0)
{
  fParticleSource  = new G4GeneralParticleSource;
  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
 
  G4ParticleDefinition* particle
                      = particleTable->FindParticle("neutron");

  fParticleSource->SetParticleDefinition(particle);
}
  PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleSource;
}
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  fParticleSource->GeneratePrimaryVertex(anEvent);
}

