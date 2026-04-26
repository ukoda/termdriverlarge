/*=================================================

  ESP32 WT32-SC01 3.5" LCD module display stand

  (c) David Annett
  10 January 2024

  A simple L shaped stand for WT32-SC01 module.

=================================================*/

// Imported modules

include <metrichardware.scad>;
include <4_lcd_msp4030.scad>;
include <PicoUltimate.scad>;

// Control rendering quality

$fa = 1;
$fs = 0.1;
Overlap = 0.05;
showsample = false;

// What parts to render

ShowNonPrint   = false;
ShowPanel      = true;
ShowPico       = true;
ShowSerialCon  = true;

// What features are present

LCDScrewType   = "Insert";  // Can be Tap or Insert

// Major dimensions

StandThickness =   3.0;
StandBorder    =  16;
FootDepth      =  70;
FootAngle      = 15;
FootTweak1     =   2;
FootTweak2     =  10.5;
PicoMountX     =  8;

// Derived dimemsions

StandWidth     = 4_LCDActiveWidth + StandBorder * 2;
StandHeight    = 4_LCDActiveHeight + StandBorder * 2;
LCDRecess      = StandThickness / 2;
PicoOffsetY    = (FootDepth - PicoUltLength) / 2 - StandThickness;
PicoOffsetZ    = (StandThickness + PicoUltThickness) / 2 + PicoUltPinsDepth;
PicoPCBTop     = PicoUltThickness + PicoUltPinsDepth;
PicoMountY     = StandThickness * 2;
PicoMountFrontZ = StandThickness * 2;

// === The stand ===

// Front face

if (ShowPanel) {
  difference() {
    union() {
      
      // Front face
      
      translate([0, StandThickness/-2, 0])
        cube([StandWidth, StandThickness, StandHeight], center = true);

      // Base
  
      translate([0, FootDepth/2-FootTweak1, StandHeight/-2+FootTweak2])
      rotate([FootAngle, 0, 0]) {

        // Main foot plate
        
        cube([StandWidth, FootDepth, StandThickness], center = true);
        
        // Front left pico hold
        
        translate([PicoUltWidth / -2,
                   FootDepth/2 - StandThickness - PicoUltLength,
                   (StandThickness + PicoMountFrontZ) / 2 - Overlap])
          cube([PicoMountX, PicoMountY, PicoMountFrontZ], center = true);
        
        // Front right pico hold
  
        translate([PicoUltWidth / 2,
                   FootDepth/2 - StandThickness - PicoUltLength,
                   (StandThickness + PicoMountFrontZ) / 2 - Overlap])
          cube([PicoMountX, PicoMountY, PicoMountFrontZ], center = true);

        // Rear left pico hold
  
        difference() {
          translate([(PicoUltWidth + M3Insert + 1 - M3PillarDiameter) / -2,
                     (FootDepth - M3PillarDiameter) / 2,
                     (StandThickness + PicoPCBTop) / 2 - Overlap])
            cube([M3PillarDiameter, M3PillarDiameter, PicoPCBTop], center = true);

          translate([(PicoUltWidth + M3Insert + 1) / -2,
                     (FootDepth - M3PillarDiameter) / 2,
                     (StandThickness + PicoPCBTop) / 2 - Overlap])
            cylinder(h = PicoPCBTop + Overlap, d = M3Insert + 0.4, center = true);
        }

        // Rear right pico hold
        
        difference() {
          translate([(PicoUltWidth + M3Insert + 1 - M3PillarDiameter) / 2,
                     (FootDepth - M3PillarDiameter) / 2,
                     (StandThickness + PicoPCBTop) / 2 - Overlap])
            cube([M3PillarDiameter, M3PillarDiameter, PicoPCBTop], center = true);

          translate([(PicoUltWidth + M3Insert + 1) / 2,
                     (FootDepth - M3PillarDiameter) / 2,
                     (StandThickness + PicoPCBTop) / 2 - Overlap])
            cylinder(h = PicoPCBTop + Overlap, d = M3Insert + 0.4, center = true);
        }

        // Left pico hold screw
        
        translate([(PicoUltWidth + M3Insert + 1) / -2,
                   (FootDepth - M3PillarDiameter) / 2,
                   (StandThickness + PicoPCBTop) / 2 - Overlap])
          M3Pillar(PicoPCBTop);

        // Right pico hold screw
        
        translate([(PicoUltWidth + M3Insert + 1) / 2,
                   (FootDepth - M3PillarDiameter) / 2,
                   (StandThickness + PicoPCBTop) / 2 - Overlap])
          M3Pillar(PicoPCBTop);
      }
    }

    // LCD cutout

    translate([0, -LCDRecess, 0])
    4_LCDModule();
        
    // Face cutout

    4_LCDModuleActiveCutout(StandThickness);
    
    // Mounting holes
    
    4_LCDModuleHountHoles(StandThickness * 2);
    
    // Pico module cutout

    translate([0, FootDepth/2-FootTweak1, StandHeight/-2+FootTweak2])
    rotate([FootAngle, 0, 0]) {
      translate([0, PicoOffsetY, PicoOffsetZ])
      rotate([0, 0, 180])
      PicoUltPCBCutout();
    }
  }
}



// Show LCD module in place in different colour

if (ShowNonPrint) {
  if (ShowPanel) {
    translate([0, -LCDRecess, 0])
    4_LCDModule();
  }
}



// Show Pico module in place in different colour

if (ShowNonPrint) {
  if (ShowPico) {
    translate([0, FootDepth/2-FootTweak1, StandHeight/-2+FootTweak2])
    rotate([FootAngle, 0, 0]) {
      translate([0, PicoOffsetY, PicoOffsetZ])
      rotate([0, 0, 180])
      PicoUltModule();
    }
  }
}

