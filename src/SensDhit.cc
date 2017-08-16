#include "SensDhit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh" 
#include <iomanip>
    
    G4ThreadLocal G4Allocator<SensDhit>* SensDhitAllocator;

    SensDhit::SensDhit()  {}

    SensDhit::~SensDhit() {}

    SensDhit::SensDhit(const SensDhit& right)
      : G4VHit()
    {
      edep        = right.edep;
      stepLength  = right.stepLength;
      processname = right.processname;
      {
       if (edep != 0)
         {
          particleDefinition = right.particleDefinition;
          if (particleDefinition == "neutron")
          {
          KinEn  = right.KinEn;
          momentum      = right.momentum;
          }
         }
      }
    }
    const SensDhit& SensDhit::operator=(const SensDhit& right)
    {
      edep        = right.edep;
      stepLength = right.stepLength;
      processname = right.processname;
      {
       if (edep != 0)
         {
          particleDefinition = right.particleDefinition;
          if (particleDefinition == "neutron")
          {
          KinEn  = right.KinEn;
          momentum      = right.momentum;
          }
         }
      }
      return *this;
    }
    G4int SensDhit::operator==(const SensDhit& right) const
    {
      return ( this == &right ) ? 1 : 0;
    }
    void SensDhit::Print()
    {
      G4cout
         << " edep: " 
         << G4BestUnit(edep,"Energy")
         << " process name: " 
         << processname << G4endl
         << " particle definition: "
         << particleDefinition << G4endl
         << " energy before: "
         << G4BestUnit( KinEn, "Energy")
         << G4endl;
    }

