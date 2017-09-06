#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Element.hh"
#include "G4Isotope.hh"
#include "SensD.hh"

#include "GB01BOptrChangeCrossSection.hh"
#include "GB02BOptrMultiParticleForceCollision.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "DetectorConstructionMessenger.hh"

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fCheckOverlaps(true), absoSDname("Clyc/ClycSD"), HDPEthickness_in(4),
  sphere(false)
{
  DetMess = new DetectorConstructionMessenger(this);
  DefineMaterials();
}
DetectorConstruction::~DetectorConstruction()
{ }

void DetectorConstruction::DefineMaterials()
{}

void DetectorConstruction::SetHDPEthickness_in(G4double val)
{HDPEthickness_in = val;}

void DetectorConstruction::SetSphere(bool flag){ sphere = flag; }

void DetectorConstruction::UpdateGeometry(){
    //Copying the SABREMC approach.
    G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

void DetectorConstruction::ConstructSDandField(){
    SensD* threadSD = new SensD ( absoSDname );
    G4SDManager *SDmanager=G4SDManager::GetSDMpointer();
    SDmanager->AddNewDetector( threadSD );
    SetSensitiveDetector("ClycLV", threadSD);

/*
    //Bias neutron cross section in target:
    G4LogicalVolume* logicTarget = G4LogicalVolumeStore::GetInstance()->GetVolume("targetLV");
    G4LogicalVolume* logicCLYC =
        G4LogicalVolumeStore::GetInstance()->GetVolume("ClycLV");
    // ----------------------------------------------
    // -- bias operator creation and attachment to volume:
    // ----------------------------------------------
    GB01BOptrChangeCrossSection* biasOperator = 
      new GB01BOptrChangeCrossSection("proton");
    GB02BOptrMultiParticleForceCollision* testMany =  
      new GB02BOptrMultiParticleForceCollision();
    biasOperator->AttachTo(logicTarget);
    testMany->AddParticle("neutron");
    testMany->AttachTo(logicCLYC);
    G4cout << " Attaching biasing operator " << biasOperator->GetName()
         << " to logical volume " << logicTarget->GetName()
         << G4endl;
    G4cout << " Attaching biasing operator " << testMany->GetName()
         << " to logical volume " << logicCLYC->GetName()
         << G4endl;
*/
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  

  // Parameters
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
      
  // World
  G4double world_sizeXY = 70*cm;
  G4double world_sizeZ  = 70*cm;
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       world_sizeXY, world_sizeXY, world_sizeZ); //its size   
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        air,         //its material
                        "World");            //its name                             
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      fCheckOverlaps);       // checking overlaps 
            
  // build clyc material
  G4Element* caesium = nist->FindOrBuildElement(55);
  G4Isotope* isoLi6 = 
    new G4Isotope("Li6", 3, 6, 6.015121*g/mole);
  G4Isotope* isoLi7 = 
    new G4Isotope("Li7", 3, 7, 7.016002*g/mole);
  G4Element* enrichedLi = 
    new G4Element("enriched Li", "Li", 2);
  enrichedLi->AddIsotope(isoLi6, 95.*perCent);
  enrichedLi->AddIsotope(isoLi7, 05.*perCent);
  G4Element* yttrium = nist->FindOrBuildElement(39);
  G4Element* chlorine = nist->FindOrBuildElement(17);
  G4Material* Cs2LiYCl6 =
    new G4Material("CLYC", 3.31*g/cm3, 4);
  G4int number_of_atoms;
  Cs2LiYCl6->AddElement(caesium, number_of_atoms=2);
  Cs2LiYCl6->AddElement(enrichedLi, number_of_atoms=1);
  Cs2LiYCl6->AddElement(yttrium, number_of_atoms=1);
  Cs2LiYCl6->AddElement(chlorine, number_of_atoms=6);           

  G4Material* HDPE = nist->FindOrBuildMaterial("G4_POLYETHYLENE");

  // clyc
  G4double clyc_radius = 1.27*cm;
  G4double clyc_dZ = 1.27*cm;    
  G4Tubs* Clyc =
    new G4Tubs("Clyc", 0., clyc_radius, clyc_dZ, 0., twopi);
  G4LogicalVolume* logicClyc =                         
    new G4LogicalVolume(Clyc,        //its solid
                        Cs2LiYCl6,         //its material
                        "ClycLV");        //its name

  // place clyc in world
  // Rotation matrix
  G4double rotation_angle = 0;//67.5;
  // Define straight line distance between front of CLYC and middle of world
  // (cm).
  G4double CLYCdistance = (129.9+25./2)/10;
  // Defne straight line distance between front of HDPE and middle of world
  // (cm).
  G4double HDPEdistance = (1.8 + 25./2)/10;
  //Total HDPE moderator thickness, in inches.
  //G4double HDPEthickness_in = 4; 
 
  G4Tubs* SrcHolder = new G4Tubs("SrcHolder", 0., 25*mm, 12.5*mm, 0, twopi);
  G4LogicalVolume* SrcHolder_log = new G4LogicalVolume(SrcHolder, HDPE,
                                                       "SrcHolder_log");
  G4ThreeVector srcpos = G4ThreeVector(0, 229.19*mm, 0);
  G4RotationMatrix srcrot = G4RotationMatrix();
  G4Transform3D srctransform = G4Transform3D(srcrot, srcpos);
  new G4PVPlacement(srctransform, SrcHolder_log, "SrcHolder_phys", logicWorld, 
                    false, 0, fCheckOverlaps); 

  G4Tubs* srcvoid = new G4Tubs("srcvoid", 0., 12.5*mm, 3*mm, 0, twopi);
  G4LogicalVolume* srcvoid_log = new G4LogicalVolume(srcvoid, air,
    "srcvoid_log");
  G4Transform3D nulltransform = G4Transform3D();
  new G4PVPlacement(nulltransform, srcvoid_log, "srcvoid_phys", SrcHolder_log,
                    false, 0, fCheckOverlaps);
 
  //I won't bother with the Al source plate (transparent to neutrons).

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Matt's Code.
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  G4ThreeVector position1 = G4ThreeVector(-(CLYCdistance+1.27+0.3)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+1.27+0.3)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm1  = G4RotationMatrix();
  rotm1.rotateY((180-rotation_angle)*deg); 
  rotm1.rotateX(0*deg); 
  rotm1.rotateZ(0*deg); 
  G4Transform3D transform1 = G4Transform3D(rotm1, position1);  
  // Place matrix
  new G4PVPlacement(transform1,                       //no rotation
                    logicClyc,               //its logical volume
                    "Clyc",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 

  ////// CLYC Casing
  //// Pointy bit
  // Define Geometry
  G4double FrontInnerRadius = 2.867*cm;
  G4double FrontOuterRadius = 3.167*cm;
  G4double BackInnerRadius = 1.378*cm;
  G4double BackOuterRadius = 1.678*cm;
  G4double PBDepth = 0.88*cm;
  G4double startingangle = 0.0*deg;
  G4double endingangle = 360.0*deg;  
  G4Material* PointyBit_mat = nist->FindOrBuildMaterial("G4_Al");
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Check what the material is
  G4Cons* PointyBit = 
    new G4Cons("PointyBit", FrontInnerRadius, FrontOuterRadius, BackInnerRadius, BackOuterRadius, PBDepth, startingangle, endingangle);
  G4LogicalVolume* logicPointyBit =                         
    new G4LogicalVolume(PointyBit,             //its solid
                        PointyBit_mat,         //its material
                        "PointyBit");        //its name
  G4Cons* PointyBit_sub = 
    new G4Cons("PointyBit_sub", 0, FrontOuterRadius, 0, BackOuterRadius, PBDepth, startingangle, endingangle);
  // Rotation Matrix
  G4ThreeVector position12 = G4ThreeVector(-(CLYCdistance+0.88)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+0.88)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm12  = G4RotationMatrix();
  rotm12.rotateY((180-rotation_angle)*deg); 
  rotm12.rotateX(0*deg); 
  rotm12.rotateZ(0*deg); 
  G4Transform3D transform12 = G4Transform3D(rotm12, position12);  
  // Place in world
  new G4PVPlacement(transform12,                       //no rotation
                    logicPointyBit,               //its logical volume
                    "PointyBit",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  //// Pointy Bit Cap (PBC)
  // Define Geometry
  G4double PBC_radius = 1.378*cm;
  G4double PBC_dZ = 0.15*cm;
  G4Material* PBC_mat = nist->FindOrBuildMaterial("G4_Al");
  G4Tubs* PBC =
    new G4Tubs("PBC", 0., PBC_radius, PBC_dZ, 0., twopi);
  G4LogicalVolume* logicPBC =                         
    new G4LogicalVolume(PBC,        //its solid
                        PBC_mat,         //its material
                        "PBCLV");        //its name
  // Rotation Matrix
  G4ThreeVector position13 = G4ThreeVector(-(CLYCdistance+0.15)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+0.15)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm13  = G4RotationMatrix();
  rotm13.rotateY((180-rotation_angle)*deg); 
  rotm13.rotateX(0*deg); 
  rotm13.rotateZ(0*deg); 
  G4Transform3D transform13 = G4Transform3D(rotm13, position13);  
  // Place on world
  new G4PVPlacement(transform13,                       //no rotation
                    logicPBC,               //its logical volume
                    "PBC",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 

  ////First Cylinder Bit (FCB) (make hollow) (wall 3mm)
  // Define Geometry
  G4double FCBO_radius = 3.167*cm;
  G4double FCBI_radius = 2.867*cm;
  G4double FCB_dZ = 0.545*cm; 
  
  G4Material* FCB_mat = nist->FindOrBuildMaterial("G4_Al");
  G4Tubs* FCB = 
    new G4Tubs("FCB", FCBI_radius, FCBO_radius, FCB_dZ, 0., twopi);
  G4LogicalVolume* logicFCB =                         
    new G4LogicalVolume(FCB,             //its solid
                        FCB_mat,         //its material
                        "FCB");        //its name
  // Rotation Matrix
  G4ThreeVector position14 = G4ThreeVector(-(CLYCdistance+2.85-.545)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85-.545)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm14  = G4RotationMatrix();
  rotm14.rotateY((180-rotation_angle)*deg); 
  rotm14.rotateX(0*deg); 
  rotm14.rotateZ(0*deg); 
  G4Transform3D transform14 = G4Transform3D(rotm14, position14);  
  // Place on world
  new G4PVPlacement(transform14,                       //no rotation
                    logicFCB,               //its logical volume
                    "FCB",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 

  //// Middle bit of case  (make hollow) (MBOC)
  // Define Geometry
  G4double MBOCO_radius = 3.167*cm;
  G4double MBOCI_radius = 2.867*cm;
  G4double MBOC_dZ = 4.865*cm; 
  
  G4Material* MBOC_mat = nist->FindOrBuildMaterial("G4_Al");
  G4Tubs* MBOC = 
    new G4Tubs("MBOC", MBOCI_radius, MBOCO_radius, MBOC_dZ, 0., twopi);
  G4LogicalVolume* logicMBOC =                         
    new G4LogicalVolume(MBOC,             //its solid
                        MBOC_mat,         //its material
                        "MBOC");        //its name  
  // Rotation Matrix
  G4ThreeVector position15 = G4ThreeVector(-(CLYCdistance+2.85+4.865)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+4.865)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm15  = G4RotationMatrix();
  rotm15.rotateY((180-rotation_angle)*deg); 
  rotm15.rotateX(0*deg); 
  rotm15.rotateZ(0*deg); 
  G4Transform3D transform15 = G4Transform3D(rotm15, position15);  
  // Place on world
  new G4PVPlacement(transform15,                       //no rotation
                    logicMBOC,               //its logical volume
                    "MBOC",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  //// Glass pane (GlassPane)
  // Define Geometry
  G4double GlassPane_radius = 2.867*cm;
  G4double GlassPane_dZ = 0.1*cm; 
  
  G4Material* GlassPane_mat = nist->FindOrBuildMaterial("G4_Pyrex_Glass");
  G4Tubs* GlassPane = 
    new G4Tubs("GlassPane", 0, GlassPane_radius, GlassPane_dZ, 0., twopi);
  G4LogicalVolume* logicGlassPane =                         
    new G4LogicalVolume(GlassPane,             //its solid
                        GlassPane_mat,         //its material
                        "GlassPane");        //its name  
  // Rotation Matrix
  G4ThreeVector position16 = G4ThreeVector(-(CLYCdistance+2.85+0.1)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+0.1)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm16  = G4RotationMatrix();
  rotm16.rotateY((180-rotation_angle)*deg); 
  rotm16.rotateX(0*deg); 
  rotm16.rotateZ(0*deg); 
  G4Transform3D transform16 = G4Transform3D(rotm16, position16);  
  // Place on world
  new G4PVPlacement(transform16,                       //no rotation
                    logicGlassPane,               //its logical volume
                    "GlassPane",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  //// End bit of ClYC (EBOC)
  // Define Geometry
  G4double EBOCO_radius = 3.167*cm;
  G4double EBOCI_radius = 0.0*cm;
  G4double EBOC_dZ = .385*cm; 
  
  G4Tubs* EBOC = 
    new G4Tubs("EBOC", EBOCI_radius, EBOCO_radius, EBOC_dZ, 0., twopi);
  G4LogicalVolume* logicEBOC =                         
    new G4LogicalVolume(EBOC,             //its solid
                        HDPE,         //its material
                        "EBOC");        //its name  
  // Rotation Matrix
  G4ThreeVector position17 = G4ThreeVector(-(CLYCdistance+2.85+10.5-.385)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+10.5-.385)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm17  = G4RotationMatrix();
  rotm17.rotateY((180-rotation_angle)*deg); 
  rotm17.rotateX(0*deg); 
  rotm17.rotateZ(0*deg); 
  G4Transform3D transform17 = G4Transform3D(rotm17, position17);  
  // Place on world
  new G4PVPlacement(transform17,                       //no rotation
                    logicEBOC,               //its logical volume
                    "EBOC",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  //// Vacuum in ClYC chamber (VICC)
  G4Element* nitrogen = 
    new G4Element("nitrogen", "N", 7, 14.0067*g/mole);  
  G4Material* Vacuum =
    new G4Material("Vacuum", 1e-20*g/cm3, 1);
  G4int number;
  Vacuum->AddElement(nitrogen, number=1);  
  // Define Geometry
  G4double VICC_radius = 2.867*cm;
  G4double VICC_dZ = 4.665*cm; 
  
  G4Tubs* VICC = 
    new G4Tubs("VICC", 0.0, VICC_radius, VICC_dZ, 0., twopi);
  G4LogicalVolume* logicVICC =                         
    new G4LogicalVolume(VICC,             //its solid
                        Vacuum,//VICC_mat,         //its material
                        "VICC");        //its name  
  // Rotation Matrix
  G4ThreeVector position0 = G4ThreeVector(-(CLYCdistance+2.85+0.2+4.665)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+0.2+4.665)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm0  = G4RotationMatrix();
  rotm0.rotateY((180-rotation_angle)*deg); 
  rotm0.rotateX(0*deg); 
  rotm0.rotateZ(0*deg); 
  G4Transform3D transform0 = G4Transform3D(rotm0, position0);  
  // Place on world
  new G4PVPlacement(transform0,                       //no rotation
                    logicVICC,               //its logical volume
                    "VICC",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  //// Second glass pane (SecondGlassPane)
  // Define Geometry
  G4double SecondGlassPane_radius = 2.867*cm;
  G4double SecondGlassPane_dZ = 0.1*cm; 
  
  G4Material* SecondGlassPane_mat = nist->FindOrBuildMaterial("G4_Pyrex_Glass");
  G4Tubs* SecondGlassPane = 
    new G4Tubs("SecondGlassPane", 0, SecondGlassPane_radius, SecondGlassPane_dZ, 0., twopi);
  G4LogicalVolume* logicSecondGlassPane =                         
    new G4LogicalVolume(SecondGlassPane,             //its solid
                        SecondGlassPane_mat,         //its material
                        "SecondGlassPane");        //its name  
  // Rotation Matrix
  G4ThreeVector position18 = G4ThreeVector(-(CLYCdistance+2.85+10.5-.77-0.1)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+10.5-.77-0.1)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm18  = G4RotationMatrix();
  rotm18.rotateY((180-rotation_angle)*deg); 
  rotm18.rotateX(0*deg); 
  rotm18.rotateZ(0*deg); 
  G4Transform3D transform18 = G4Transform3D(rotm18, position18);  
  // Place on world
  new G4PVPlacement(transform18,                       //no rotation
                    logicSecondGlassPane,               //its logical volume
                    "SecondGlassPane",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 

  //// Plastic chunk at end
  /// Smaller Cylinder at End (SCAE)
  // Define Geometry
  G4double SCAEO_radius = 2.788*cm;
  G4double SCAEI_radius = 0.0*cm;
  G4double SCAE_dZ = 0.636*cm; 
  
  G4Tubs* SCAE = 
    new G4Tubs("SCAE", SCAEI_radius, SCAEO_radius, SCAE_dZ, 0., twopi);
  G4LogicalVolume* logicSCAE =                         
    new G4LogicalVolume(SCAE,             //its solid
                        HDPE,         //its material
                        "SCAE");        //its name  
  // Rotation Matrix
  G4ThreeVector position19 = G4ThreeVector(-(CLYCdistance+2.85+10.5+0.636)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+10.5+0.636)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm19  = G4RotationMatrix();
  rotm19.rotateY((180-rotation_angle)*deg); 
  rotm19.rotateX(0*deg); 
  rotm19.rotateZ(0*deg); 
  G4Transform3D transform19 = G4Transform3D(rotm19, position19);  
  // Place on world
  new G4PVPlacement(transform19,                       //no rotation
                    logicSCAE,               //its logical volume
                    "SCAE",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  /// Bigger Cylinder at End (BCAE)
  // Define Geometry
  G4double BCAEO_radius = 3.209*cm;
  G4double BCAEI_radius = 2.859*cm;
  G4double BCAE_dZ = 1.25*cm; 
  
  G4Tubs* BCAE = 
    new G4Tubs("BCAE", BCAEI_radius, BCAEO_radius, BCAE_dZ, 0., twopi);
  G4LogicalVolume* logicBCAE =                         
    new G4LogicalVolume(BCAE,             //its solid
                        HDPE,         //its material
                        "BCAE");        //its name  
  // Rotation Matrix
  G4ThreeVector position20 = G4ThreeVector(-(CLYCdistance+2.85+10.5+1.272+1.25)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+10.5+1.272+1.25)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm20  = G4RotationMatrix();
  rotm20.rotateY((180-rotation_angle)*deg); 
  rotm20.rotateX(0*deg); 
  rotm20.rotateZ(0*deg); 
  G4Transform3D transform20 = G4Transform3D(rotm20, position20);  
  // Place on world
  new G4PVPlacement(transform20,                       //no rotation
                    logicBCAE,               //its logical volume
                    "BCAE",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  /// Plastic End Cover (PEC)
  // Define Geometry
  G4double PEC_radius = 2.859*cm;
  G4double PEC_dZ = .175*cm; 
  
  G4Tubs* PEC = 
    new G4Tubs("PEC", 0.0, PEC_radius, PEC_dZ, 0., twopi);
  G4LogicalVolume* logicPEC =                         
    new G4LogicalVolume(PEC,             //its solid
                        HDPE,         //its material
                        "PEC");        //its name  
  // Rotation Matrix
  G4ThreeVector position21 = G4ThreeVector(-(CLYCdistance+2.85+10.5+1.272+2.5-.175)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+10.5+1.272+2.5-.175)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm21  = G4RotationMatrix();
  rotm21.rotateY((180-rotation_angle)*deg); 
  rotm21.rotateX(0*deg); 
  rotm21.rotateZ(0*deg); 
  G4Transform3D transform21 = G4Transform3D(rotm21, position21);  
  // Place on world
  new G4PVPlacement(transform21,                       //no rotation
                    logicPEC,               //its logical volume
                    "PEC",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 


  ////// CLYC Holder
  //// Big Metal Pole ClYC (BMPC)
  // Geometry
  G4double BMPC_width = 1.0*cm;
  G4double BMPC_height = 9.7*cm;
  G4double BMPC_depth = 1.0*cm;

  G4Material* BMPC_mat = nist->FindOrBuildMaterial("G4_Al");
  G4Box* BMPC = new G4Box("BMPC", BMPC_width, BMPC_height, BMPC_depth);  
  G4LogicalVolume* logicBMPC =                         
    new G4LogicalVolume(BMPC,             //its solid
                        BMPC_mat,         //its material
                        "BMPCLV");        //its name
  
  //// Main Black Bit ClYC (MBBC)
  // Define Geometry
  G4double MBBCO_radius = 3.467*cm;
  G4double MBBCI_radius = 3.167*cm;
  G4double MBBC_dZ = 1.03*cm; 
  
  G4Tubs* MBBC = 
    new G4Tubs("MBBC", MBBCI_radius, MBBCO_radius, MBBC_dZ, 0., twopi);
  G4LogicalVolume* logicMBBC =                         
    new G4LogicalVolume(MBBC,             //its solid
                        HDPE,         //its material
                        "MBBC");        //its name  
  // Rotation Matrix
  G4ThreeVector position23 = G4ThreeVector(-(CLYCdistance+2.85+1.61+1.61)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+1.61+1.61)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm23  = G4RotationMatrix();
  rotm23.rotateY((180-rotation_angle)*deg); 
  rotm23.rotateX(0*deg); 
  rotm23.rotateZ(0*deg); 
  G4Transform3D transform23 = G4Transform3D(rotm23, position23);  
  // Place on world
  new G4PVPlacement(transform23,                       //no rotation
                    logicMBBC,               //its logical volume
                    "MBBC",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 

  //// Black overlap bit CLYC
  /// Front fallange for CLYC (FFFC)
  // Define Geometry
  G4double FFFCO_radius = 4.047*cm;
  G4double FFFCI_radius = 3.167*cm;
  G4double FFFC_dZ = .29*cm; 
  
  G4Tubs* FFFC = 
    new G4Tubs("FFFC", FFFCI_radius, FFFCO_radius, FFFC_dZ, 0., twopi);
  G4LogicalVolume* logicFFFC =                         
    new G4LogicalVolume(FFFC,             //its solid
                        HDPE,         //its material
                        "FFFC");        //its name  
  // Rotation Matrix
  G4ThreeVector position24 = G4ThreeVector(-(CLYCdistance+2.85+1.61+.29)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+1.61+.29)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm24 = G4RotationMatrix();
  rotm24.rotateY((180-rotation_angle)*deg); 
  rotm24.rotateX(0*deg); 
  rotm24.rotateZ(0*deg); 
  G4Transform3D transform24 = G4Transform3D(rotm24, position24);  
  // Place on world
  new G4PVPlacement(transform24,                       //no rotation
                    logicFFFC,               //its logical volume
                    "FFFC",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
 /// Back Fallange for CLYC (BFFC)
  // Define Geometry
  G4double BFFCO_radius = 4.047*cm;
  G4double BFFCI_radius = 3.167*cm;
  G4double BFFC_dZ = .29*cm; 
  
  G4Tubs* BFFC = 
    new G4Tubs("BFFC", BFFCI_radius, BFFCO_radius, BFFC_dZ, 0., twopi);
  G4LogicalVolume* logicBFFC =                         
    new G4LogicalVolume(BFFC,             //its solid
                        HDPE,         //its material
                        "BFFC");        //its name  
  // Rotation Matrix
  G4ThreeVector position25 = G4ThreeVector(-(CLYCdistance+2.85+1.61+1.61+1.32)*(sin(rotation_angle*deg))*cm,229.19*mm,((CLYCdistance+2.85+1.61+1.61+1.32)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotm25 = G4RotationMatrix();
  rotm25.rotateY((180-rotation_angle)*deg); 
  rotm25.rotateX(0*deg); 
  rotm25.rotateZ(0*deg); 
  G4Transform3D transform25 = G4Transform3D(rotm25, position25);  
  // Place on world
  new G4PVPlacement(transform25,                       //no rotation
                    logicBFFC,               //its logical volume
                    "BFFC",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  //// Circular metal Bits CLYC (CMBC)
  // Define Geometry
  G4double CMBCO_radius = 4.157*cm;
  G4double CMBCI_radius = 3.467*cm;
  G4double CMBC_dZ = 1.0*cm; 
  
  G4Material* CMBC_mat = nist->FindOrBuildMaterial("G4_Al");
  G4Tubs* CMBC = 
    new G4Tubs("CMBC", CMBCI_radius, CMBCO_radius, CMBC_dZ, 0., twopi);
  G4LogicalVolume* logicCMBC =                         
    new G4LogicalVolume(CMBC,             //its solid
                        CMBC_mat,         //its material
                        "CMBC");        //its name  


  //// Make pole and circle into union solid
  // Rotation matrix
  G4ThreeVector positionMetalholderC = G4ThreeVector(-(CLYCdistance+2.85+1.61+1.61)*(sin(rotation_angle*deg))*cm,9.7*cm,((CLYCdistance+2.85+1.61+1.61)*(cos(rotation_angle*deg)))*cm);
  G4RotationMatrix rotmMetalholderC  = G4RotationMatrix();
  rotmMetalholderC.rotateY((180-rotation_angle)*deg); 
  rotmMetalholderC.rotateX(0*deg); 
  rotmMetalholderC.rotateZ(0*deg); 
  G4Transform3D transformMetalholderC = G4Transform3D(rotmMetalholderC, positionMetalholderC);  
  // Build Metal holder
  G4ThreeVector MetalholderCtrans = G4ThreeVector(0., 13.219*cm, 0.);
  G4RotationMatrix* MetalholderCRot = new G4RotationMatrix;
    MetalholderCRot->rotateY(0.);

  G4UnionSolid* MetalholderC = 
    new G4UnionSolid("MetalholderC", BMPC, CMBC,  MetalholderCRot, MetalholderCtrans);
  G4LogicalVolume* logicMetalholderC =                         
    new G4LogicalVolume(MetalholderC,             //its solid
                        CMBC_mat,             //its material
                        "MetalholderC");        //its name
    new G4PVPlacement(transformMetalholderC,                     //no rotation
                    logicMetalholderC,          //its logical volume
                    "MetalholderC",             //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 

  if(!sphere){
  	if(HDPEthickness_in>0){
  	    ////// HDPE
  	    //// Weird endy bit (WEB)
  	    // Define Geometry
  	    G4double WEB_radius = 3.99*cm;
  	    G4double WEB_dZ = 0.15*cm; 
  	
  	    G4Tubs* WEB = 
  	      new G4Tubs("WEB", 0.0, WEB_radius, WEB_dZ, 0., twopi);
  	    G4LogicalVolume* logicWEB =                         
  	      new G4LogicalVolume(WEB,             //its solid
  	                          HDPE,         //its material
  	                          "WEB");        //its name  
  	    // Rotation Matrix
  	    G4ThreeVector position27 = G4ThreeVector(-(HDPEdistance+0.15)*(sin(rotation_angle*deg))*cm,229.19*mm,((HDPEdistance+0.15)*(cos(rotation_angle*deg)))*cm);
  	    G4RotationMatrix rotm27 = G4RotationMatrix();
  	    rotm27.rotateY((180-rotation_angle)*deg); 
  	    rotm27.rotateX(0*deg); 
  	    rotm27.rotateZ(0*deg); 
  	    G4Transform3D transform27 = G4Transform3D(rotm27, position27);  
  	    // Place on world
  	    new G4PVPlacement(transform27,                       //no rotation
  	                      logicWEB,               //its logical volume
  	                      "WEB",                  //its name
  	                      logicWorld,              //its mother  volume
  	                      false,                   //no boolean operation
  	                      0,                       //copy number
  	                      fCheckOverlaps);         // checking overlaps 
  	    //// Main cylinder of HDPE (MCOH)
  	    // Define Geometry
  	    G4double MCOH_radius = 3.12*cm;
  	    G4double MCOH_dZ = ((HDPEthickness_in/2)*2.62-0.15);         //// Note: change 10.0 to the depth of the HDPE plug
  	
  	    G4Tubs* MCOH = 
  	      new G4Tubs("MCOH", 0.0, MCOH_radius, MCOH_dZ*cm, 0., twopi);
  	    G4LogicalVolume* logicMCOH =                         
  	      new G4LogicalVolume(MCOH,             //its solid
  	                          HDPE,         //its material
  	                          "MCOH");        //its name  
  	    // Rotation Matrix
  	    G4ThreeVector position28 = G4ThreeVector(-(HDPEdistance+0.3+(MCOH_dZ))*(sin(rotation_angle*deg))*cm,229.19*mm,((HDPEdistance+0.3+(MCOH_dZ))*(cos(rotation_angle*deg)))*cm);
  	    G4RotationMatrix rotm28 = G4RotationMatrix();
  	    rotm28.rotateY((180-rotation_angle)*deg); 
  	    rotm28.rotateX(0*deg); 
  	    rotm28.rotateZ(0*deg); 
  	    G4Transform3D transform28 = G4Transform3D(rotm28, position28);  
  	    // Place on world
  	    new G4PVPlacement(transform28,                       //no rotation
  	                      logicMCOH,               //its logical volume
  	                      "MCOH",                  //its name
  	                      logicWorld,              //its mother  volume
  	                      false,                   //no boolean operation
  	                      0,                       //copy number
  	                      fCheckOverlaps);         // checking overlaps 

  	}

  	////// HDPE Holder
  	//// Big Metal Pole HDPE (BMPH)
  	// Geometry
  	G4double BMPH_width = 1.0*cm;
  	G4double BMPH_height = 9.7*cm;
  	G4double BMPH_depth = 1.0*cm;
  	G4Material* BMPH_mat = nist->FindOrBuildMaterial("G4_Al");
  	G4Box* BMPH = new G4Box("BMPH", BMPH_width, BMPH_height, BMPH_depth);  
  	G4LogicalVolume* logicBMPH =                         
  	  new G4LogicalVolume(BMPH,             //its solid
  	                      BMPH_mat,         //its material
  	                      "BMPHLV");        //its name
  	//// Main Black Bit HDPE (MBBH)
  	// Define Geometry
  	G4double MBBHO_radius = 3.42*cm;
  	G4double MBBHI_radius = 3.12*cm;
  	G4double MBBH_dZ = 1.03*cm;   
  	G4Tubs* MBBH = 
  	  new G4Tubs("MBBH", MBBHI_radius, MBBHO_radius, MBBH_dZ, 0., twopi);
  	G4LogicalVolume* logicMBBH =                         
  	  new G4LogicalVolume(MBBH,             //its solid
  	                      HDPE,         //its material
  	                      "MBBH");        //its name  
  	// Rotation Matrix
  	G4ThreeVector position30 = G4ThreeVector(-(HDPEdistance+0.3+1.03+.29*2)*(sin(rotation_angle*deg))*cm,229.19*mm,((HDPEdistance+0.3+1.03+.29*2)*(cos(rotation_angle*deg)))*cm);
  	G4RotationMatrix rotm30  = G4RotationMatrix();
  	rotm30.rotateY((180-rotation_angle)*deg); 
  	rotm30.rotateX(0*deg); 
  	rotm30.rotateZ(0*deg); 
  	G4Transform3D transform30 = G4Transform3D(rotm30, position30);  
  	// Place on world
  	new G4PVPlacement(transform30,                       //no rotation
  	                  logicMBBH,               //its logical volume
  	                  "MBBH",                  //its name
  	                  logicWorld,              //its mother  volume
  	                  false,                   //no boolean operation
  	                  0,                       //copy number
  	                  fCheckOverlaps);         // checking overlaps 
  	//// Black overlap bit HDPE
  	/// Front fallange for HDPE (FFFH)
  	// Define Geometry
  	G4double FFFHO_radius = 4.30*cm;             
  	G4double FFFHI_radius = 3.42*cm;             
  	G4double FFFH_dZ = .29*cm; 
  	
  	G4Tubs* FFFH = 
  	  new G4Tubs("FFFH", FFFHI_radius, FFFHO_radius, FFFH_dZ, 0., twopi);
  	G4LogicalVolume* logicFFFH =                         
  	  new G4LogicalVolume(FFFH,             //its solid
  	                      HDPE,         //its material
  	                      "FFFH");        //its name  
  	// Rotation Matrix
  	G4ThreeVector position31 = G4ThreeVector(-(HDPEdistance+0.3+.29)*(sin(rotation_angle*deg))*cm,229.19*mm,((HDPEdistance+0.3+.29)*(cos(rotation_angle*deg)))*cm);
  	G4RotationMatrix rotm31 = G4RotationMatrix();
  	rotm31.rotateY((180-rotation_angle)*deg); 
  	rotm31.rotateX(0*deg); 
  	rotm31.rotateZ(0*deg); 
  	G4Transform3D transform31 = G4Transform3D(rotm31, position31);  
  	// Place on world
  	new G4PVPlacement(transform31,                       //no rotation
  	                  logicFFFH,               //its logical volume
  	                  "FFFH",                  //its name
  	                  logicWorld,              //its mother  volume
  	                  false,                   //no boolean operation
  	                  0,                       //copy number
  	                  fCheckOverlaps);         // checking overlaps 
  	/// Back Fallange for HDPE (BFFH)
  	// Define Geometry
  	G4double BFFHO_radius = 4.30*cm;
  	G4double BFFHI_radius = 3.42*cm;
  	G4double BFFH_dZ = .29*cm; 
  	
  	G4Tubs* BFFH = 
  	  new G4Tubs("BFFH", BFFHI_radius, BFFHO_radius, BFFH_dZ, 0., twopi);
  	G4LogicalVolume* logicBFFH =                         
  	  new G4LogicalVolume(BFFH,             //its solid
  	                      HDPE,         //its material
  	                      "BFFH");        //its name  
  	// Rotation Matrix
  	G4ThreeVector position32 = G4ThreeVector(-(HDPEdistance+0.3+1.03*2+.29*3)*(sin(rotation_angle*deg))*cm,229.19*mm,((HDPEdistance+0.3+1.03*2+.29*3)*(cos(rotation_angle*deg)))*cm);
  	G4RotationMatrix rotm32 = G4RotationMatrix();
  	rotm32.rotateY((180-rotation_angle)*deg); 
  	rotm32.rotateX(0*deg); 
  	rotm32.rotateZ(0*deg); 
  	G4Transform3D transform32 = G4Transform3D(rotm32, position32);  
  	// Place on world
  	new G4PVPlacement(transform32,                       //no rotation
  	                  logicBFFH,               //its logical volume
  	                  "BFFH",                  //its name
  	                  logicWorld,              //its mother  volume
  	                  false,                   //no boolean operation
  	                  0,                       //copy number
  	                  fCheckOverlaps);         // checking overlaps 
  	//// Circular metal Bits HDPE (CMBH)
  	// Define Geometry
  	G4double CMBHO_radius = 4.11*cm;
  	G4double CMBHI_radius = 3.42*cm;
  	G4double CMBH_dZ = 1.0*cm; 
  	
  	G4Material* CMBH_mat = nist->FindOrBuildMaterial("G4_Al");
  	G4Tubs* CMBH = 
  	  new G4Tubs("CMBH", CMBHI_radius, CMBHO_radius, CMBH_dZ, 0., twopi);
  	G4LogicalVolume* logicCMBH =                         
  	  new G4LogicalVolume(CMBH,             //its solid
  	                      CMBH_mat,         //its material
  	                      "CMBH");        //its name  

  	//// Make pole and circle into union solid
  	// Rotation matrix
  	G4ThreeVector positionMetalholderH = G4ThreeVector(-(HDPEdistance+0.3+1.03+.29*2)*(sin(rotation_angle*deg))*cm,9.7*cm,((HDPEdistance+0.3+1.03+.29*2)*(cos(rotation_angle*deg)))*cm);
  	G4RotationMatrix rotmMetalholderH  = G4RotationMatrix();
  	rotmMetalholderH.rotateY((180-rotation_angle)*deg); 
  	rotmMetalholderH.rotateX(0*deg); 
  	rotmMetalholderH.rotateZ(0*deg); 
  	G4Transform3D transformMetalholderH = G4Transform3D(rotmMetalholderH, positionMetalholderH);  
  	// Build Metal holder
  	G4ThreeVector MetalholderHtrans = G4ThreeVector(0., 13.219*cm, 0.);
  	G4RotationMatrix* MetalholderHRot = new G4RotationMatrix;
  	  MetalholderHRot->rotateY(0.);
  	G4UnionSolid* MetalholderH = 
  	  new G4UnionSolid("MetalholderH", BMPH, CMBH, MetalholderHRot, MetalholderHtrans);
  	G4LogicalVolume* logicMetalholderH =                         
  	  new G4LogicalVolume(MetalholderH,             //its solid
  	                      CMBH_mat,             //its material
  	                      "MetalholderH");        //its name
  	  new G4PVPlacement(transformMetalholderH,                     //no rotation
  	                  logicMetalholderH,          //its logical volume
  	                  "MetalholderH",             //its name
  	                  logicWorld,              //its mother  volume
  	                  false,                   //no boolean operation
  	                  0,                       //copy number
  	                  fCheckOverlaps);         // checking overlaps 
  }
  else{
	//sphere geometry.
	//Make radius = HDPEthickness_in
	G4Sphere* SimpleSphere = new G4Sphere("SimpleSphere", 0, 
				HDPEthickness_in, 0, twopi, 0, twopi);
	//Subtract solid pointy bit. Since the sphere is centred on the CLYC,
	//this doesn't need any rotation/translation.
	G4SubtractionSolid* BS_m_PB = 
		new G4SubtractionSolid("BS_m_PB", SimpleSphere, PointyBit_sub);
	//Subtract the CLYC tube. This needs to be translated by the length of
	//the 'pointy bit' (PBDepth).
	G4Tubs* SubCyl = new G4Tubs("SubCyl", 0, MBOCO_radius, 
				HDPEthickness_in, 0, twopi);
	G4SubtractionSolid* BonnerSphere =
		new G4SubtractionSolid("BonnerSphere", BS_m_PB, SubCyl, 0,
			G4ThreeVector(0, 0, -HDPEthickness_in - PBDepth*2));

	G4LogicalVolume* logicBonner = 
		new G4LogicalVolume(BonnerSphere, HDPE, "Bonner_log");

	new G4PVPlacement(transform12, logicBonner, "Bonner_phys", logicWorld,
			false, 0, fCheckOverlaps);
		
  }

  // Building the target chamber
  //// wholepipe

  G4double wholepipe_radius = 1.285*cm;
  G4double wholepipe_dZ = 20.25*cm; 
  
  G4Material* wholepipe_mat = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  G4Tubs* wholepipe = 
    new G4Tubs("wholepipe", 0., wholepipe_radius, wholepipe_dZ, 0., twopi);
  G4LogicalVolume* logicwholepipe =                         
    new G4LogicalVolume(wholepipe,             //its solid
                        wholepipe_mat,         //its material
                        "wholepipe");        //its name

  /*
  //// Putting on table (table)
  // Geometry
  G4double table_width = 70.0*cm;
  G4double table_height = 0.5*cm;
  G4double table_depth = 70.0*cm;

  G4Material* table_mat = nist->FindOrBuildMaterial("G4_Al");
  G4Box* table = new G4Box("table", table_width, table_height, table_depth);  
  G4LogicalVolume* logictable =                         
    new G4LogicalVolume(table,             //its solid
                        table_mat,         //its material
                        "tableLV");        //its name
  
  // Matrix
  G4ThreeVector position34 = G4ThreeVector(0.,-0.5*cm,0.);
  G4RotationMatrix rotm34  = G4RotationMatrix();
  rotm34.rotateY(0*deg); 
  rotm34.rotateX(0*deg); 
  rotm34.rotateZ(0*deg); 
  G4Transform3D transform34 = G4Transform3D(rotm34, position34);  
  // Place in world
  new G4PVPlacement(transform34,                       //no rotation
                    logictable,               //its logical volume
                    "table",                  //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  */

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl; 

  //always return the physical World
  return physWorld;
}
