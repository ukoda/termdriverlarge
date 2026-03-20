/*=================================================

  ESP32 WT32-SC01 3.5" LCD module display stand

  (c) David Annett
  10 January 2024

  A simple L shaped stand for WT32-SC01 module.

=================================================*/

// Imported modules

include <4_lcd_msp4030.scad>;

// Control rendering quality

$fa = 1;
$fs = 0.1;
Overlap = 0.05;
showsample = false;

// What parts to render

ShowNonPrint   = false;
ShowPanel      = true;

// What features are present

LCDScrewType   = "Insert";  // Can be Tap or Insert

// Major dimensions

StandThickness =   3.0;
StandBorder    =  16;
FootDepth      =  40;
FootAngle      = 105;
FootTweak1     =   1.1;
FootTweak2     =   6.62;

// Derived dimemsions

StandWidth     = 4_LCDActiveWidth + StandBorder * 2;
StandHeight    = 4_LCDActiveHeight + StandBorder * 2;
LCDRecess      = StandThickness / 2;

// === The stand ===

// Front face

if (ShowPanel) {
  difference() {
    union() {
      translate([0, StandThickness/-2, 0])
        cube([StandWidth, StandThickness, StandHeight], center = true);
    }

    // LCD cutout

    translate([0, -LCDRecess, 0])
    4_LCDModule();
        
    // Face cutout

    4_LCDModuleActiveCutout(StandThickness);
    
    // Mounting holes
    
    4_LCDModuleHountHoles(StandThickness * 2);
    }

// Base

    translate([0, FootDepth/2-FootTweak1, StandHeight/-2+FootTweak2])
    rotate([FootAngle, 0, 0])
        cube([StandWidth, StandThickness, FootDepth], center = true);
}



// Show module in place in different colour

if (ShowNonPrint) {
  if (ShowPanel) {
    translate([0, -LCDRecess, 0])
    4_LCDModule();
  }
}
