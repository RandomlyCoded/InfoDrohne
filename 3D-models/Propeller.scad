_l = 11.5;
_w = 2;

_d = 2;

Prop = true;
Bridge = true;
Prop2 = true;

$fn = 64;

// Propeller
if (Prop) {
    difference() {
        scale(7.5/12)
        import ("zero_pl.stl");

        translate([-_l/2, 0, 0])
        difference() {
            cube(size=_l, center=true);
            cube(size=[_l + 1, _w, _l + 1], center=true);
        }
    }
}

// bridge
if (Bridge) {
    rotate([90, 0, 0])
    translate([-_l - 5/2, 0, 2.5])
    difference () {
        cube(size = [2 * _l + 5, 10, 2], center=true);
        
        for (x = [-1, 1]) {
            translate([x * (_l - 0.5), 0, 0])
            cylinder (d = _d, h = 3, center=true);

            translate([x * (_l/2 - 1.25), 0, 0])
            cylinder (d = _d, h = 3, center=true);
        }
    }
}

if (Prop2) {
    translate([-_l * 2 - 5, 0, 0])
    rotate([0, 180, 0])
    difference() {
        scale(7.5/12)
        import ("zero_pl.stl");

        translate([-_l/2, 0, 0])
        difference() {
            cube(size=_l, center=true);
            cube(size=[_l + 1, _w, _l + 1], center=true);
        }
    }
}