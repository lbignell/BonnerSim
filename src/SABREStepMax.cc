#include "SABREStepMax.hh"

SABREStepMax::SABREStepMax(const G4String& processName)
 : G4VDiscreteProcess(processName),MaxChargedStep(DBL_MAX)
{
  //  pMess = new SABREStepMaxMessenger(this);
}
SABREStepMax::~SABREStepMax() { 
  //delete pMess; 
}
G4bool SABREStepMax::IsApplicable(const G4ParticleDefinition& particle) 
{ 
  return (particle.GetPDGCharge() != 0.);
}
void SABREStepMax::SetMaxStep(G4double step) {MaxChargedStep = step;}
G4double SABREStepMax::PostStepGetPhysicalInteractionLength(const G4Track&,
                                                  G4double,
                                                  G4ForceCondition* condition )
{
  // condition is set to "Not Forced"
  *condition = NotForced;
  return MaxChargedStep;
}
G4VParticleChange* SABREStepMax::PostStepDoIt(const G4Track& aTrack, const G4Step&)
{
   // do nothing
   aParticleChange.Initialize(aTrack);
   return &aParticleChange;
}

