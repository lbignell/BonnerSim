#include "StackingAction.hh"
#include "G4Track.hh"
#include "G4NeutrinoE.hh"
#include "G4VProcess.hh"
using namespace std;
StackingAction::StackingAction()
{ }

StackingAction::~StackingAction()
{ }

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* track)
{
  //keep primary particle
  if (track->GetParentID() == 0) return fUrgent;
  if (track->GetDefinition() == G4NeutrinoE::NeutrinoE()) return fKill;
  /*if ((track->GetDefinition()->GetPDGCharge() != 0)&&
      ((track->GetVertexPosition()).x()>-200)&&//Need to use crappy position cut rather than volume cut due to seg faults.
      //(track->GetMaterial()->GetName()!="CLYC")&&//Causes seg fault
      (track->GetKineticEnergy() < 1)){
      	if(track->GetDefinition()->GetParticleName() == "e+") { return fKill; }
  }*/
  //  if (track->GetVolume()->GetName()==
  return fUrgent;
}
