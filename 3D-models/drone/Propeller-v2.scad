$fn = 64;

module changeHole(d) {
    difference() {
        scale(10/14)
        import("corejo/helice-toroidale/helice_18.stl");

        cylinder (d = d, h = 10);
    }
}

for (i = [0 : 10]) {
    _d = 4 + 0.05 * i;

    translate([0, i * 50, 0])
    changeHole();
    
    translate([60, i * 50, 0])
    linear_extrude(1)
    text(str(_d));
}