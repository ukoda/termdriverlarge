/*=============================================================================

  Model of Metric hardware

  (c) David Annett
  9 August 2022

  The value MxPass is for a hole that is big enough for a Mx part to pass thru
  
  The value MxTap is for a hole that is small enough to have a thread tapped
  for a Mx part.  For screws below M3 an insert will be needed instead of a tap.
  
  The function MxPillar creates a mount pillar with a hole for a threaded insert
  be inserted under presure.
 
=============================================================================*/

// Imported modules

//use <SomeLibrary.scad>;

// Control rendering quality

$fa        = 1;
$fs        = 0.1;

Overlap    = 0.05;
showsample = true;

/*-----------------------------------------------------------------------------

  Hole sizes

  Pass   - Size to pass bolt through easily with little gap
  Tap    - Size to tap a thread into with thread cutting tool
  Insert - Size to hole a threaded brass insert

-----------------------------------------------------------------------------*/

M2Pass              =  2.3;
M2Insert            =  3.4;
M2PillarLength      =  3;
M2PillarDiameter    =  5;

M2_5Pass            =  2.8;
M2_5Insert          =  3.4;
M2_5PillarLength    =  3;
M2_5PillarDiameter  =  5;

M3Pass              =  3.3;
M3Tap               =  2.5;
M3Insert            =  3.9;
M3PillarLength      =  4.9;
M3PillarDiameter    =  6.5;
M3RoundHead         =  5.3;
M3RoundHeadPass     =  5.5;

M4Pass              =  4.4;

M5Pass              =  5.5;

VESA75Spacing       = 75;
VESA75Pass          = M4Pass;

VESA100Spacing      = 100;
VESA100Pass         = M4Pass;

MOverlap            =  0.1;


module M2Pillar (
  Height
  )
{
  difference() {
      cylinder(h = Height + MOverlap, d = M2PillarDiameter, center = true);
    
      // Insert hole

      translate([0, 0, MOverlap])
          cylinder(h = Height, d = M2Insert, center = true);
    
      // Insert lead in
  
      translate([0, 0, MOverlap + Height / 2 - 0.5])
        cylinder(h = 0.5, d = M2Insert + 0.4, center = true);
  }
}


module M2_5Pillar (
  Height
  )
{
  difference() {
      cylinder(h = Height + MOverlap, d = M2_5PillarDiameter, center = true);
    
      // Insert hole

      translate([0, 0, MOverlap])
          cylinder(h = Height, d = M2_5Insert, center = true);
    
      // Insert lead in
  
      translate([0, 0, MOverlap + Height / 2 - 0.5])
        cylinder(h = 0.5, d = M2_5Insert + 0.4, center = true);
  }
}


module M3Pillar (
  Height
  )
{
  difference() {
      cylinder(h = Height + MOverlap, d = M3PillarDiameter, center = true);
    
      // Insert hole

      translate([0, 0, MOverlap])
          cylinder(h = Height, d = M3Insert, center = true);
    
      // Insert lead in

      translate([0, 0, MOverlap + Height / 2 - 0.5])
        cylinder(h = 0.5, d = M3Insert + 0.4, center = true);
  }
}


/*
  VESA 75 pass holes
  Centered on center of pattern, projects in -Y direction
*/
module VESA75Holes (
  Depth
)
{
  translate([VESA75Spacing/2, Depth/-2, VESA75Spacing/2])
  rotate([90, 0 , 0])
    cylinder(h = Depth, d = VESA75Pass, center = true);

  translate([VESA75Spacing/2, Depth/-2, VESA75Spacing/-2])
  rotate([90, 0 , 0])
    cylinder(h = Depth, d = VESA75Pass, center = true);

  translate([VESA75Spacing/-2, Depth/-2, VESA75Spacing/2])
  rotate([90, 0 , 0])
    cylinder(h = Depth, d = VESA75Pass, center = true);

  translate([VESA75Spacing/-2, Depth/-2, VESA75Spacing/-2])
  rotate([90, 0 , 0])
    cylinder(h = Depth, d = VESA75Pass, center = true);
}


// === Show samples ===

if (showsample) {
  M3Pillar(5);
}
