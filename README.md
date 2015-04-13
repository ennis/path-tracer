# path-tracer
Small path tracer experiment based on smallpt and pbrt

Most of the formulas used are probably physically wrong, but the algorithm produces interesting images nevertheless.

Potentially interesting features include:
- Support for HDR environment maps
- Diffuse, purely specular & glass BRDFs
- Depth of field (kind of)
- Area & point lights (w/ explicit light sampling)
- Triangle meshes (slow, without acceleration structures)
- Progressive rendering
- Supersampling

Requires SFML2 

## Screenshots

![](https://github.com/ennis/path-tracer/raw/master/screenshots/diffuse2.png "Soft shadows with an area light source (approx 1000 samples per pixel)")
![](https://github.com/ennis/path-tracer/raw/master/screenshots/envmap.png "Environment maps & meshes")
![](https://github.com/ennis/path-tracer/raw/master/screenshots/early_indirect.png "Early version with different surface types")
