#include "primitivesBuilder.h"


// MeshData CreateCube(float size) {
//     MeshData mesh;
//     float s = size / 2.0f;

//     // Define the 24 vertices (4 per face x 6 faces)
//     // Format: {Position(x,y,z), Normal(nx,ny,nz), UV(u,v)}
//     mesh.vertices = {
//         // Front face (Z+)
//         {{-s, -s,  s}, {0, 0, 1}, {0, 0}}, {{ s, -s,  s}, {0, 0, 1}, {1, 0}},
//         {{ s,  s,  s}, {0, 0, 1}, {1, 1}}, {{-s,  s,  s}, {0, 0, 1}, {0, 1}},
//         // Back face (Z-)
//         {{-s, -s, -s}, {0, 0,-1}, {1, 0}}, {{-s,  s, -s}, {0, 0,-1}, {1, 1}},
//         {{ s,  s, -s}, {0, 0,-1}, {0, 1}}, {{ s, -s, -s}, {0, 0,-1}, {0, 0}},
//         // Top face (Y+)
//         {{-s,  s, -s}, {0, 1, 0}, {0, 1}}, {{-s,  s,  s}, {0, 1, 0}, {0, 0}},
//         {{ s,  s,  s}, {0, 1, 0}, {1, 0}}, {{ s,  s, -s}, {0, 1, 0}, {1, 1}},
//         // Bottom face (Y-)
//         {{-s, -s, -s}, {0,-1, 0}, {0, 0}}, {{ s, -s, -s}, {0,-1, 0}, {1, 0}},
//         {{ s, -s,  s}, {0,-1, 0}, {1, 1}}, {{-s, -s,  s}, {0,-1, 0}, {0, 1}},
//         // Right face (X+)
//         {{ s, -s, -s}, {1, 0, 0}, {1, 0}}, {{ s,  s, -s}, {1, 0, 0}, {1, 1}},
//         {{ s,  s,  s}, {1, 0, 0}, {0, 1}}, {{ s, -s,  s}, {1, 0, 0}, {0, 0}},
//         // Left face (X-)
//         {{-s, -s, -s}, {-1,0, 0}, {0, 0}}, {{-s, -s,  s}, {-1,0, 0}, {1, 0}},
//         {{-s,  s,  s}, {-1,0, 0}, {1, 1}}, {{-s,  s, -s}, {-1,0, 0}, {0, 1}}
//     };

//     // Define the 36 indices (6 indices per face)
//     for (int i = 0; i < 6; ++i) {
//         unsigned int offset = i * 4;
//         mesh.indices.push_back(offset + 0);
//         mesh.indices.push_back(offset + 1);
//         mesh.indices.push_back(offset + 2);

//         mesh.indices.push_back(offset + 0);
//         mesh.indices.push_back(offset + 2);
//         mesh.indices.push_back(offset + 3);
//     }

//     return mesh;
// }