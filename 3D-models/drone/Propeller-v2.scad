module changeHole(d) {
    import("corejo/helice-toroidale/helice_18.stl");

    $fn = 64;

    difference() {
        cylinder (d = 6, h = 10);
        cylinder (d = d, h = 10.01);
    }
}