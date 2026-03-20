/*=================================================

  Colour 4" LCD module - MSP4030 variant (blue PCB)

  (c) David Annett
  17 Mar 2026

  The reference point for everythimg is the front
  face of the panel centered on the active image
  area.

=================================================*/

// Imported modules

include <metrichardware.scad>;

// Control rendering quality

$fa = 1;
$fs = 0.1;
Overlap = 0.05;
showsample = true;


// Major dimensions

4_LCDWidth          = 108;    // X - Width of LCD PCB
4_LCDHeight         =  60.88; // Z - Height of LCD PCB
4_LCDXPCBOffset     =   2.65; // X
4_LCDMountOffset    =   3;    // XZ - Offset for mounting holes from corner
4_LCDPCBDepth       =   1.6;  // Y - PCB thickness

4_LCDPanelWidth     =  95.07; // X Added 0.5mm for cutout clearance
4_LCDPanelDepth     =   2.5;  // Y Depth of actual panel
4_LCDPanelHeight    =  61.38; // Z Added 0.5mm for cutout clearance
4_LCDXPanelOffset   =   2.65; // X Panel shift to the right

4_LCDActiveWidth    =  83.52; // X 
4_LCDActiveHeight   =  55.68; // Z 

4_LCDPinsWidth      =   2;    // X
4_LCDPinsHeight     =  35;    // Z
4_LCDPinsDepth      =   1.5;  // Y
4_LCDPinsLeftOffset =   1;    // X To edge of pins, not center
4_LCDPinsTopOffset  =  13;    // Z To edge of pins, not center


// Derived dimemsions

4_LCDDepth = 4_LCDPanelDepth + 4_LCDPCBDepth; // Y - Panel face to PCB rear 



module 4_LCDModuleActiveCutout(depth)
{
  translate([0, depth/-2, 0])
    cube([4_LCDActiveWidth,
          depth + Overlap * 2,
          4_LCDActiveHeight],
          center = true);
}



module 4_LCDModuleHountHoles(depth)
{
  fdepth = depth + Overlap * 2;
  translate([4_LCDWidth/2 - 4_LCDMountOffset - 4_LCDXPanelOffset,
             0,
             4_LCDHeight/2 - 4_LCDMountOffset])
  rotate([90, 0, 0])
    cylinder(fdepth, d=M3Pass, center = true);

  translate([4_LCDWidth/-2 + 4_LCDMountOffset - 4_LCDXPanelOffset,
             0,
             4_LCDHeight/2 - 4_LCDMountOffset])
  rotate([90, 0, 0])
    cylinder(fdepth, d=M3Pass, center = true);

  translate([4_LCDWidth/2 - 4_LCDMountOffset - 4_LCDXPanelOffset,
             0,
             4_LCDHeight/-2 + 4_LCDMountOffset])
  rotate([90, 0, 0])
    cylinder(fdepth, d=M3Pass, center = true);

  translate([4_LCDWidth/-2 + 4_LCDMountOffset- 4_LCDXPanelOffset,
             0,
             4_LCDHeight/-2 + 4_LCDMountOffset])
  rotate([90, 0, 0])
    cylinder(fdepth, d=M3Pass, center = true);
}




// === The LCD display ===

// Show module in place, centered on front face of active area
module 4_LCDModule(
  colour = "red"
  )
{
  // PCB
  
  color(colour)
    difference() {
      translate([-(4_LCDXPCBOffset),
                 4_LCDPanelDepth + 4_LCDPCBDepth / 2,
                 0])
        cube([4_LCDWidth,
              4_LCDPCBDepth,
              4_LCDHeight],
             center = true);
      
      translate([0,
                 4_LCDPanelDepth + 4_LCDPCBDepth / 2,
                 0])
        4_LCDModuleHountHoles(4_LCDPCBDepth);
    }

  // Panel
  
  color(colour)
  translate([-(4_LCDXPanelOffset),
             4_LCDPanelDepth / 2,
             0])
    cube([4_LCDPanelWidth,
          4_LCDPanelDepth,
          4_LCDPanelHeight],
         center = true);

  // Connector pins
    
  color(colour)
  translate([(4_LCDWidth / -2 - 4_LCDXPCBOffset + 4_LCDPinsWidth / 2 + 4_LCDPinsLeftOffset),
             4_LCDPinsDepth / -2 + 4_LCDPanelDepth + Overlap,
             4_LCDPanelHeight / 2 - 4_LCDPinsHeight / 2 - 4_LCDPinsTopOffset])
    cube([4_LCDPinsWidth,
          4_LCDPinsDepth,
          4_LCDPinsHeight],
         center = true);


  // Active area

  color("black")
  translate([0,
             -0.05,
             0])
    cube([4_LCDActiveWidth,
          0.1,
          4_LCDActiveHeight],
         center = true);
}


// === Show samples ===

if (showsample) {
  4_LCDModule();
}
//4_LCDModuleActiveCutout(2);
//4_LCDModuleHountHoles(4_LCDPCBDepth);