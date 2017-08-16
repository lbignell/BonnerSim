#include "EventAction.hh"
#include "RunAction.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "SensD.hh"
#include "G4ThreeVector.hh"
#include "SensDhit.hh"
#include "Analysis.hh"

EventAction::EventAction(RunAction* runAction)
 : G4UserEventAction(), 
   fRunAction(runAction),
   fCollID_clyc(-1)
{}
EventAction::~EventAction()
{ }
void EventAction::BeginOfEventAction(const G4Event* /*evt*/)
{ }

void EventAction::EndOfEventAction(const G4Event* evt )
{ 
    G4SDManager* sdm = G4SDManager::GetSDMpointer();
    SensD* sd = (SensD*)(sdm->FindSensitiveDetector("Clyc/ClycSD"));
    
    SensDhitCollection* hc = sd->fHitsCollection;

    if(!hc){ return; }//No hits in this event.

    G4AnalysisManager* am = G4AnalysisManager::Instance();

    //check if clyc hit collection exists in this event
    if(hc){
    //Run through once to get total edep.
      G4double edep_tot = 0;
      for(G4int i = 0; i<hc->entries(); i++){
          edep_tot += (*hc)[i]->Getedep();
      }
      G4int evtnum = evt->GetEventID();

        //Now process the hits and fill the ntuple.
      for(G4int i = 0; i<hc->entries(); i++){
          SensDhit* ahit = (*hc)[i];
          am->FillNtupleDColumn(0, 0, ahit->Getedep());         //ntuple id = 0, col id = 0
          am->FillNtupleSColumn(0, 1, ahit->Getprocessname());  //ntuple id = 0, col id = 1
          am->FillNtupleDColumn(0, 2, ahit->GetKinEn());
          G4ThreeVector vpos = ahit->Getvertexpos();
          am->FillNtupleDColumn(0, 3, vpos.x());
          am->FillNtupleDColumn(0, 4, vpos.y());
          am->FillNtupleDColumn(0, 5, vpos.z());
          am->FillNtupleSColumn(0, 6, ahit->GetparticleDefinition());
          G4ThreeVector vmom = ahit->Getvertexmomentum();
          am->FillNtupleDColumn(0, 7, vmom.x());
          am->FillNtupleDColumn(0, 8, vmom.y());
          am->FillNtupleDColumn(0, 9, vmom.z());
          G4ThreeVector pos = ahit->Getpos();
          am->FillNtupleDColumn(0, 10, pos.x());
          am->FillNtupleDColumn(0, 11, pos.y());
          am->FillNtupleDColumn(0, 12, pos.z());
          G4ThreeVector mom = ahit->Getmomentum();
          am->FillNtupleDColumn(0, 13, mom.x());
          am->FillNtupleDColumn(0, 14, mom.y());
          am->FillNtupleDColumn(0, 15, mom.z());
          am->FillNtupleIColumn(0, 16, evtnum);
          am->FillNtupleDColumn(0, 17, edep_tot);
          am->FillNtupleDColumn(0, 18, ahit->Getweight());
	      am->FillNtupleIColumn(0, 19, ahit->GetStepNumber());
          am->FillNtupleSColumn(0, 20, ahit->GetCreatorProcess());
     	  am->FillNtupleIColumn(0, 21, ahit->GetTrackID());
          am->FillNtupleDColumn(0, 22, ahit->GetVertexKinEn());
          am->AddNtupleRow(0);
      }
    }
}
