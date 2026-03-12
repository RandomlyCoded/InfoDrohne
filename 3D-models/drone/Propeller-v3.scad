sizeTest = true;

$fn = 64;

module hexagon(r) {
    translate([0, 0, -5])
    linear_extrude(10)
    polygon([for (i = [0 : 6]) r * [cos(i * 60), sin(i * 60)]]);
}

module Hole(d) {
    difference() {
        scale(10/14)
        import("corejo/helice-toroidale/helice_18.stl");

        translate([0, 0, 4])
        cylinder(12, d=11.8, center=true);
    }
    
    difference() {
        cylinder(8.2, d=11.8);

        cylinder(8.2, d=d);
    }
}

if (sizeTest) {
    difference() {
        translate([0, 0, 2.5])
        cube([15, 15, 5], true);
        
        translate([-4, -4, 0])
        cylinder(8.2, d=5);

        translate([-4,  4, 0])
        cylinder(8.2, d=5.1);

        translate([ 4,  4, 0])
        cylinder(8.2, d=5.2);
        
        translate([ 4, -4, 0])
        cylinder(8.2, d=4.9);
        
        translate([5, -10.5, 4])
        linear_extrude(2)
        text("-");
    }
    
    translate([20, 0, 0])
    difference() {
        cylinder(8.2, d=11.8);
        cylinder(8.2, d=5.1);
    }
} else {
    *Hole(5.2);
    
    //translate([30, 0, 0])
    difference() {
        union() {
            translate([-75, 0, 0])
            cube([150, 10, 10], true);
            
            cube([20, 20, 10], true);
        }
        #hexagon(7.9 / (2 * cos(30)) + 0.2);
    }
}
