#include "SensD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include <iostream>
#include <fstream>

using namespace std;
SensD::SensD(const G4String& name)   
     : G4VSensitiveDetector(name), fHCID(-1)
{ 
    SDname = name;
    collectionName.insert("CLYC_HC");
}

SensD::~SensD(){} 

void SensD::Initialize(G4HCofThisEvent* hce){
    fHitsCollection = new SensDhitCollection(SDname, collectionName[0]);
    if(fHCID<0){
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection->GetName());
    }
    hce->AddHitsCollection(fHCID, fHitsCollection);
    /*G4cout << "SENSITIVE DETECTOR::" << G4endl
           << "Hit Collection ptr: " << hce << G4endl
           << "# Collections: " << hce->GetNumberOfCollections() << G4endl
           << "Capacity: " << hce->GetCapacity() << G4endl
           << "fHCID" << fHCID << G4endl
           << "HC ptr: " << hce->GetHC(fHCID) << G4endl
           << "HC[0]: " << hce->GetHC(0) << G4endl 
           << G4endl;*/
} 

G4bool SensD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4double edep = step->GetTotalEnergyDeposit();
  SensDhit* ahit = new SensDhit();
  ahit->Setedep(edep);

  G4Track* track = step->GetTrack();
  const G4String & processname = 
     step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  ahit->Setprocessname(processname);
  ahit->SetKinEn(track->GetKineticEnergy());
  ahit->SetparticleDefinition(track->GetParticleDefinition()->GetParticleName());
  ahit->Setvertexpos(track->GetVertexPosition());
  ahit->Setvertexmom(track->GetVertexMomentumDirection());
  ahit->Setposition(track->GetPosition());
  ahit->Setmomentum(track->GetMomentumDirection());
  ahit->Setweight(track->GetWeight());
  ahit->SetStepNumber(track->GetCurrentStepNumber());
  if(track->GetCreatorProcess())
  {ahit->SetCreatorProcess(track->GetCreatorProcess()->GetProcessName());}
  else{ahit->SetCreatorProcess("");}
  ahit->SetTrackID(track->GetTrackID());
  ahit->SetVertexKinEn(track->GetVertexKineticEnergy());
  fHitsCollection->insert(ahit);
  return true;
}

void SensD::EndOfEvent(G4HCofThisEvent*)
{
   /*ofstream myfile;
   myfile.open ("Important_Info.txt", std::ofstream::out |
   std::ofstream::app);
   if ( verboseLevel>1 ) { 
     auto nofHits = fHitsCollection->entries();
       myfile
       << G4endl 
       << "-------->Hits Collection: in this event they are " << nofHits 
       << " hits in the tracker chambers: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }
   myfile.close();*/
}
