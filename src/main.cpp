#include "includes/ktypes.h"
#include "includes/krender.h"
#include "includes/kio.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

int main(int argc, char ** argv) {
    config_t cfg = parse_cli_input(argc, argv);
    Model model = Model(cfg.obj_file);

    //TGAImage wireframe    = draw_wireframe(model, cfg, white); // Draws wireframe
    //TGAImage   gouraud      = apply_gouraud_shade_no_z_buffer(model, cfg);   // Applies Gouraud shading without z-buffering
    //TGAImage   z_buffered   = triangle_fill_random_colors(model, cfg);
    TGAImage  t = apply_gouraud_shade_z_buffer(model, cfg);


    //save_result(wireframe, "output-wireframe.tga");
    //save_result(gouraud,   "output-gouraud.tga");
    //save_result(z_buffered,   "output-random-colors.tga");
    save_result(t,   "output-random-colors.tga");
    return 0;
}
