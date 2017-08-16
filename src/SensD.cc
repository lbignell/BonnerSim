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
   /*ofstream myfile;
   myfile.open ("Important_Info.txt", std::ofstream::out |
   std::ofstream::app);
   // energy deposit
   auto edep = step->GetTotalEnergyDeposit();
   G4Track* track = step->GetTrack();
   
   const G4String & processname = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

   // add (vector<pair<string, double>>;)
   vector<pair<string, double>> EdepVec;
   // particle type
   auto particleDefinition = track->GetDefinition()->GetParticleName();
   myfile << "particleDefinition = " << particleDefinition << endl;
   G4cout << "particleDefinition = " << particleDefinition << G4endl;
   bool newptype = true;
   for(vector<pair<string, double>>::iterator it = EdepVec.begin(); it != EdepVec.end(); ++it){
    //Code to look at first pair member, compare the two strings, want the result of this code to be the input of the next bit of code
     bool compar = false;
     //char a;
     //char b;
     auto a = particleDefinition.c_str();
     auto b = (*it).first.c_str();
 myfile << "Looking in vector at type: " << b << endl;
     if (strcmp(a,b) == 0)
         {
            compar = true;
        myfile << "Found a match!" << endl;
          }
      if(compar){
 	//increment energy (*it).second
 	newptype = false;
 }
Add new line to vector if new particle type

   if (newptype == true)
 {  
     // declare that 'particle' is a pair
     pair <std::string,double> particle;
     // say what is in the pair
     particle = std::make_pair(particleDefinition, edep);        
     // make 'particle' be the pair to be added to the vector
     EdepVec.emplace_back (particle);
     // output to check that the new particle has been added
     myfile << "New particle!" << endl;
     myfile << "Particle name: " << particle.first << endl;
     myfile << "Energy deposit: " << particle.second << endl;
   }    
     if (particleDefinition == "neutron")

   // Getting energy before process
   auto EnergyBefore = track->GetTotalEnergy();  
   myfile << "Energy Before: " << EnergyBefore << G4endl; 
   
   const G4String & processname = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  myfile << "Process Name: " << processname <<G4endl;


  G4RunManager* runManager = new G4RunManager;
  // Get the pointer to the Run manager
  G4RunManager* RunManager = G4RunManager::GetRunManager();
  const G4Event* currentevent = runManager->GetCurrentEvent();
  G4PrimaryVertex* primaryvertex = currentevent->GetPrimaryVertex();
  G4PrimaryParticle* primaryparticle = primaryvertex->GetPrimary();
  myfile << "Neutron Vertex: " << primaryvertex << G4endl;

  auto momentum = primaryparticle->GetMomentum();
  myfile << "Neutron Momentum: " << momentum << G4endl;  

 }
  myfile << "Particle definition: " << particleDefinition << G4endl;  
   // step length
   G4double stepLength = 0.;
   if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
     stepLength = step->GetStepLength();
   }
   if ( edep==0. && stepLength == 0. ) return false;      
   auto touchable = (step->GetPreStepPoint()->GetTouchable());
   // Get calorimeter cell id 
   auto layerNumber = touchable->GetReplicaNumber(1); 
   // Get hit accounting data for this cell
   auto hit = (*fHitsCollection)[layerNumber];
  if ( ! hit ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hit " << layerNumber; 
    G4Exception("SensD::ProcessHits()",
      "MyCode0004", FatalException, msg);
  }         
  // Get hit for total accounting
  auto hitTotal 
    = (*fHitsCollection)[fHitsCollection->entries()-1];
  // Add values
  hit->Add(edep, stepLength);
  hitTotal->Add(edep, stepLength);  
  myfile.close();*/
  G4double edep = step->GetTotalEnergyDeposit();
  //if(edep==0){ return true; }
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
  //ahit->Print();
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
