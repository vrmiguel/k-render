#ifndef __KRENDER_MAIN_H
#define __KRENDER_MAIN_H

#include <vector>
#include "ktypes.h"
#include "kvec.h"
using std::vector;

class Model {
public:
    vector<Vec3f>       verts;
    vector<vector<int>> faces;
    Model(const char *filename);
    void rotate(float theta);
};

TGAImage apply_gouraud_shade_z_buffer(Model model, config_t cfg);
TGAImage triangle_fill_random_colors(Model model, config_t cfg);
TGAImage apply_gouraud_shade_no_z_buffer(Model model, config_t cfg);
void     draw_triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
TGAImage draw_wireframe(Model model, config_t cfg, TGAColor c);
void     draw_line(s32 x0, s32 y0, s32 x1, s32 y1, TGAImage &image, TGAColor color);

#endif // __KRENDER_MAIN_H
