module changeHole(d) {
    scale(14/10)
    import("corejo/helice-toroidale/helice_18.stl");

    $fn = 64;

    difference() {
        cylinder (d = 6, h = 10);

        cylinder (d = d, h = 10);
    }
}

changeHole(4);

translate([0, 50, 0])
changeHole(3.9);