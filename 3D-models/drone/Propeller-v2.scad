sizeTest = true;

module hexagon(r) {
    translate([0, 0, -5])
    linear_extrude(10)
    polygon([for (i = [0 : 6]) r * [cos(i * 60), sin(i * 60)]]);
}

r = 7.9 / (2 * cos(30));

if (sizeTest) {
    difference () {
        cube([50, 50, 5], true);
        
        translate([-15, -15, 0])
        hexagon(r);

        translate([-15,  15, 0])
        hexagon(r + 0.1);

        translate([ 15,  15, 0])
        hexagon(r + 0.2);

        translate([ 15, -15, 0])
        hexagon(r - 0.1);
        
        translate([20, -25, 1.5])
        linear_extrude(2)
        text("-");
    }
} else {
    difference() {
        scale(10/14)
        import("corejo/helice-toroidale/helice_18.stl");
        
        // Das Loch muss nicht ganz durchgehen; daher sollten 5mm ausreichen
        hexagon(r + 0.2);
    }
}
