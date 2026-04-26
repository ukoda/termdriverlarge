/*=================================================

  Ultimate Pico module

  (c) David Annett
  25 April 2026


=================================================*/

// Imported modules

//include <metrichardware.scad>;

// Control rendering quality

$fa        = 1;
$fs        = 0.1;

Overlap    = 0.05;
Clearance  = 0.5;
showsample = true;


// Major dimensions

PicoUltWidth          =  21;    // X - Width of PCB
PicoUltLength         =  53.8;  // Y - Length of PCB
PicoUltThickness      =   1.6;  // Z - Thicknes of PCB, excluding componets

PicoUltUSBWidth       =   9;    // X
PicoUltUSBLength      =   7.7;  // Y
PicoUltUSBThickness   =   3.2;  // Z

PicoUltPinsWidth      =   2;    // X
PicoUltPinsLength     =  50;    // Y
PicoUltPinsProgLength =  10;    // X
PicoUltPinsDepth      =   1.5;  // Z
PicoUltPinsEdgeOffset  =  1;    // X To edge of pins, not center
PicoUltPinsFrontOffset =  3.3;  // Y To edge of pins, not center


// Derived dimemsions




// === The module ===

// Show module in place, centered on front face of active area
module PicoUltModule(
  colour = "green"
  )
{
  // PCB
  
  color(colour)
  cube([PicoUltWidth,
        PicoUltLength,
        PicoUltThickness],
       center = true);

  // Connector pins
    
  color("grey") {
    translate([(PicoUltWidth - PicoUltPinsWidth) / -2 + PicoUltPinsEdgeOffset,
               (PicoUltLength - PicoUltPinsLength) / -2 + PicoUltPinsFrontOffset,
               (PicoUltThickness + PicoUltPinsDepth) / 2])
      cube([PicoUltPinsWidth,
            PicoUltPinsLength,
            PicoUltPinsDepth],
           center = true);

    translate([(PicoUltWidth - PicoUltPinsWidth) / -2 + PicoUltPinsEdgeOffset,
               (PicoUltLength - PicoUltPinsLength) / -2 + PicoUltPinsFrontOffset,
               (PicoUltThickness + PicoUltPinsDepth) / -2])
      cube([PicoUltPinsWidth,
            PicoUltPinsLength,
            PicoUltPinsDepth],
           center = true);

    translate([(PicoUltWidth - PicoUltPinsWidth) / 2 - PicoUltPinsEdgeOffset,
               (PicoUltLength - PicoUltPinsLength) / -2 + PicoUltPinsFrontOffset,
               (PicoUltThickness + PicoUltPinsDepth) / 2])
      cube([PicoUltPinsWidth,
            PicoUltPinsLength,
            PicoUltPinsDepth],
           center = true);

    translate([(PicoUltWidth - PicoUltPinsWidth) / 2 - PicoUltPinsEdgeOffset,
               (PicoUltLength - PicoUltPinsLength) / -2 + PicoUltPinsFrontOffset,
               (PicoUltThickness + PicoUltPinsDepth) / -2])
      cube([PicoUltPinsWidth,
            PicoUltPinsLength,
            PicoUltPinsDepth],
           center = true);

    translate([0,
               (PicoUltLength - PicoUltPinsWidth) / 2 - PicoUltPinsEdgeOffset,
               (PicoUltThickness + PicoUltPinsDepth) / 2])
      cube([PicoUltPinsProgLength,
            PicoUltPinsWidth,
            PicoUltPinsDepth],
           center = true);

    translate([0,
               (PicoUltLength - PicoUltPinsWidth) / 2 - PicoUltPinsEdgeOffset,
               (PicoUltThickness + PicoUltPinsDepth) / -2])
      cube([PicoUltPinsProgLength,
            PicoUltPinsWidth,
            PicoUltPinsDepth],
           center = true);
  }

  // USB connector
  
  color("grey") {
    translate([0,
               (PicoUltLength - PicoUltUSBLength) / -2,
               (PicoUltThickness + PicoUltUSBThickness) / 2])
      cube([PicoUltUSBWidth,
            PicoUltUSBLength,
            PicoUltUSBThickness],
           center = true);
  }
 
}


// PCB with clearance added

module PicoUltPCBCutout()
{
  // PCB
  
  cube([PicoUltWidth + Clearance,
        PicoUltLength + Clearance,
        PicoUltThickness + Clearance],
       center = true);
}


// === Show samples ===

if (showsample) {
  PicoUltModule();
}
