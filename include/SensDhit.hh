#ifndef SensDhit_h
#define SensDhit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

class SensDhit : public G4VHit
{
  public:
    SensDhit();
    SensDhit(const SensDhit&);
    virtual ~SensDhit();

    // operators
    const SensDhit& operator=(const SensDhit&);
    G4int operator==(const SensDhit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print();

    // methods to handle data
    void Add(G4double de, G4double dl);

    // get methods
    G4double Getedep() const;
    G4double GetstepLength() const;
    G4String Getprocessname() const;
    G4String GetparticleDefinition() const;
    G4double GetKinEn() const;
    G4ThreeVector Getvertexpos() const;
    G4ThreeVector Getvertexmomentum() const;
    G4ThreeVector Getpos() const;
    G4ThreeVector Getmomentum() const;
    G4double Getweight() const;
    G4int GetStepNumber() const;
    G4String GetCreatorProcess() const;
    G4int GetTrackID() const;
    G4double GetVertexKinEn() const;

    void Setedep(G4double val){edep = val;};  
    void Setprocessname(G4String val){processname = val;};  
    void SetKinEn(G4double val){KinEn = val;};  
    //void Setprimaryvertex(G4ThreeVector val){primaryvertex = val;};  
    //void Setprimarymomentum(G4ThreeVector val){primarymomentum = val;};  
    void Setvertexpos(G4ThreeVector val){vertexpos = val;};  
    void Setvertexmom(G4ThreeVector val){vertexmom = val;};  
    void Setposition(G4ThreeVector val){position = val;};  
    void Setmomentum(G4ThreeVector val){momentum = val;};  
    void SetparticleDefinition(G4String val){particleDefinition = val;};
    void Setweight(G4double val){weight = val;};  
    void SetStepNumber(G4int val){stepnum = val;};
    void SetCreatorProcess(G4String val){creatorproc = val;};
    void SetTrackID(G4int val){trackID = val;};
    void SetVertexKinEn(G4double val){vertKinEn = val;};

  private:
    G4double edep;        ///< Energy deposit in the sensitive volume
    G4double stepLength;  ///< step length in the  sensitive volume
    G4String processname; 
    G4String particleDefinition; 
    G4double KinEn;
    G4ThreeVector vertexpos; 
    G4ThreeVector vertexmom;
    G4ThreeVector position; 
    G4ThreeVector momentum; 
    G4double weight;  ///weight of track due to biasing
    G4int stepnum;
    G4String creatorproc;
    G4int trackID;
    G4double vertKinEn;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<SensDhit> SensDhitCollection;

extern G4ThreadLocal G4Allocator<SensDhit>* SensDhitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* SensDhit::operator new(size_t)
{
  if(!SensDhitAllocator)
    SensDhitAllocator = new G4Allocator<SensDhit>;
  void *hit;
  hit = (void *) SensDhitAllocator->MallocSingle();
  return hit;
}

inline void SensDhit::operator delete(void *hit)
{
  if(!SensDhitAllocator)
    SensDhitAllocator = new G4Allocator<SensDhit>;
  SensDhitAllocator->FreeSingle((SensDhit*) hit);
}

inline void SensDhit::Add(G4double de, G4double dl) {
  edep += de; 
  stepLength += dl;
}

inline G4double SensDhit::Getedep() const { 
  return edep; 
}

inline G4double SensDhit::GetstepLength() const { 
  return stepLength; 
}

inline G4String SensDhit::Getprocessname() const { 
  return processname; 
}

inline G4String SensDhit::GetparticleDefinition() const { 
  return particleDefinition; 
}

inline G4double SensDhit::GetKinEn() const { 
  return KinEn; 
}

inline G4ThreeVector SensDhit::Getvertexpos() const { 
  return vertexpos; 
}

inline G4ThreeVector SensDhit::Getvertexmomentum() const { 
  return vertexmom; 
}

inline G4ThreeVector SensDhit::Getpos() const { 
  return position; 
}

inline G4ThreeVector SensDhit::Getmomentum() const { 
  return momentum; 
}

inline G4double SensDhit::Getweight() const { 
  return weight; 
}
inline G4int SensDhit::GetStepNumber() const{
  return stepnum;
}
inline G4String SensDhit::GetCreatorProcess() const{
  return creatorproc;
}
inline G4int SensDhit::GetTrackID() const{
  return trackID;
}
inline G4double SensDhit::GetVertexKinEn() const{
  return vertKinEn;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

