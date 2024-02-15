An implementation of Mesh -> Multi resolution continuous LOD Mesh.
In spirit, a (fast) nanoNanite.
It uses explicit rather than implicit spatial partitioning because
the goal is to eventually adapt it for Earth mesh rendering, and  
out of core meshes are easier to deal with with such an approach.
