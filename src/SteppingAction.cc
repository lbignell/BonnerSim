#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"

SteppingAction::SteppingAction(){}

SteppingAction::~SteppingAction(){}

void SteppingAction::UserSteppingAction(const G4Step* stp){
	/*G4Track* trk = stp->GetTrack();
	if(trk->GetTrackID()==0){
		//Primary Proton!
		if(trk->GetKineticEnergy()<1){//Kill primaries that fall < 1 MeV.
		//trk->SetTrackStatus(fStopAndKill);
		}
	}*/
}
