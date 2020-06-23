#include "includes/krender.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

float K_FLOAT_MAX = std::numeric_limits<float>::max();


using std::swap;
using std::abs;
using std::cerr;
using std::ifstream;

// Barycentric coordinates
Vec3f get_bar_coord(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
    Vec3f s[2];

    s[1].x = C.y-A.y;
    s[1].y = B.y-A.y;
    s[1].z = A.y-P.y;

    s[0].x = C.x-A.x;
    s[0].y = B.x-A.x;
    s[0].z = A.x-P.x;

    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u.z)>1e-2)         // dont forget that u.z is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void draw_z_buf_triangle(Vec3f *pts, float *zbuffer, TGAImage &image, config_t cfg, TGAColor color) {
    Vec2f bboxmin( K_FLOAT_MAX,  K_FLOAT_MAX);
    Vec2f bboxmax(-K_FLOAT_MAX, -K_FLOAT_MAX);

    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        bboxmin.x = std::max(0.f,      std::min(bboxmin.x, pts[i].x));
        bboxmax.x = std::min(clamp.x,  std::max(bboxmax.x, pts[i].x));

        bboxmin.y = std::max(0.f,      std::min(bboxmin.y, pts[i].y));
        bboxmax.y = std::min(clamp.y,  std::max(bboxmax.y, pts[i].y));
    }
    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f bc_screen  = get_bar_coord(pts[0], pts[1], pts[2], P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z =  pts[0].z*bc_screen.x;
            P.z += pts[1].z*bc_screen.y;
            P.z += pts[2].z*bc_screen.z;

            if (zbuffer[int(P.x+P.y* cfg.width)]<P.z) {
                zbuffer[int(P.x+P.y* cfg.width)] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

Vec3f world_to_screen(Vec3f v, config_t cfg) {
    return Vec3f(int((v.x+1.)*cfg.width/2.+.5), int((v.y+1.)*cfg.height/2.+.5), v.z);
}

TGAImage triangle_fill_random_colors(Model model, config_t cfg)
{
    float *zbuffer = new float[cfg.width * cfg.height];
    for (int i=cfg.width* cfg.height; i--;)
    {
        zbuffer[i] = -K_FLOAT_MAX;
    }
    TGAImage image(cfg.width, cfg.height, ColorMode::RGB);
    for (std::vector<int> face : model.faces) {
        Vec3f pts[3];
        for (int i=0; i<3; i++) pts[i] = world_to_screen(model.verts[face[i]], cfg);
        draw_z_buf_triangle(pts, zbuffer, image, cfg, TGAColor(rand()%255, rand()%255, rand()%255, 255));
    }
    delete[] zbuffer;
    return image;
}

TGAImage apply_gouraud_shade_z_buffer(Model model, config_t cfg)
{
    float *zbuffer = new float[cfg.width * cfg.height];
    for (int i=cfg.width* cfg.height; i--;)
    {
        zbuffer[i] = -K_FLOAT_MAX;
    }
    TGAImage image(cfg.width, cfg.height, ColorMode::RGB);
    Vec3f light_dir(0,0,-1);
    for (std::vector<int> face : model.faces) {
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f v = model.verts[face[j]];
            screen_coords[j] = Vec2i((v.x+1.)*cfg.width/2., (v.y+1.)*cfg.height/2.);
            world_coords[j]  = v;
        }
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
        n.normalize();
        float intensity = n*light_dir;
        Vec3f pts[3];
        for (int i=0; i<3; i++) pts[i] = world_to_screen(model.verts[face[i]], cfg);

        if (intensity)
        {
            draw_z_buf_triangle(pts, zbuffer, image, cfg, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }
    delete[] zbuffer;
    return image;
}

TGAImage apply_gouraud_shade_no_z_buffer(Model model, config_t cfg)
{
    TGAImage image(cfg.width, cfg.height, ColorMode::RGB);
    Vec3f light_dir(0,0,-1);
    for (std::vector<int> face : model.faces)
    {
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f v = model.verts[face[j]];
            screen_coords[j] = Vec2i((v.x+1.)*cfg.width/2., (v.y+1.)*cfg.height/2.);
            world_coords[j]  = v;
        }
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
        n.normalize();
        float intensity = n*light_dir;
        if (intensity>0)
        {
            draw_triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }
    return image;
}

void Model::rotate(float theta)
{
    float cos_theta = cos(theta);
    float sin_theta = sin(theta);

    for (Vec3f & vert : verts)
    {
        float x = vert.x;
        float z = vert.z;
        vert.x = x * cos_theta - z * sin_theta;
        vert.z = z * cos_theta + x * sin_theta;
    }
}

//! Obj parser by Dmitry V. Sokolov
Model::Model(const char *filename) : verts(), faces() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail())
    {
        std::cerr << "Failed to load: " << filename << "\n";
        return;
    }
    std::string line;

    /* while we have not reached the end of the file */
    while (!in.eof()) {

        /* get one oine at a time */
        std::getline(in, line);
        /* make a stream from it */
        std::istringstream iss(line.c_str());

        char trash;

        /* if it is a line describing vertex coordinates */
        if (!line.compare(0, 2, "v "))
        {
            /* consume the 'v' symbol */
            iss >> trash;
            /* consume the coordinates */
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            /* store */
            verts.push_back(v);

        } /* if it is a line describing vertex texture coordinates */
        else if (!line.compare (0, 3, "vt "))
        {
            /* consume symbol 'v' */
            iss >> trash;
            /* consume symbol 't' */
            iss >> trash;
            /* consume the coordinates */
            Vec3f tex;
            for (int i=0;i<3;i++) iss >> tex.raw[i];
            /* store */
        //    uvw_.push_back(tex);
        }
        else if (!line.compare (0, 3, "vn "))
        {
            /* consume symbol 'v' */
            iss >> trash;
            /* consume symbol 'n' */
            iss >> trash;
            /* consume the coordinates */
            Vec3f n;
            for (int i=0;i<3;i++) iss >> n.raw[i];
            /* store */
        //    normals_.push_back(n);
        }
        else if (!line.compare(0, 2, "f ")) {
            /* current face */
            std::vector<int> f;
            std::vector<int> f_t;
            std::vector<int> f_n;

            int idx;
            int t_idx, n_idx;

            /* consume 'f' symbol */
            iss >> trash;
            while (iss >> idx >> trash >> t_idx >> trash >> n_idx) {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
                f_t.push_back(t_idx-1);
                f_n.push_back(n_idx-1);
            }
            faces.push_back(f);
        //    facestex_.push_back(f_t);
        //    facesn_.push_back(f_n);
        }
    }
    cerr << "krender: read model \"" << filename << "\" with " << verts.size() << " vertices and " << faces.size() << " faces.\n";
}

void draw_triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t0.y == t1.y && t0.y==t2.y)
    {
        // Degenerate triangle
        return;
    }

    if (t0.y>t1.y) { swap(t0, t1); }
    if (t0.y>t2.y) { swap(t0, t2); }  // Here we sort t0, t1 and t2 from lower to upper.
    if (t1.y>t2.y) { swap(t1, t2); }

    int total_height = t2.y-t0.y;

    for (int i=0; i<total_height; i++)
    {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float) i/total_height;
        float beta  = (float) (i-(second_half ? t1.y-t0.y : 0))/segment_height;
        Vec2i A = t0 + (t2-t0)*alpha;
        Vec2i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
        if (A.x>B.x) {
            swap(A, B);
        }
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, t0.y+i, color);
        }
    }
}

TGAImage draw_wireframe(Model model, config_t cfg, TGAColor c)
{
    TGAImage image(cfg.width, cfg.height, ColorMode::RGB);
    for (vector<int> face : model.faces)
    {
        for (u8 j=0; j<3; j++)
        {
            Vec3f v0 = model.verts[face[j]];
            Vec3f v1 = model.verts[face[(j+1)%3]];
            int x0 = (v0.x+1.)*cfg.width/2.;
            int y0 = (v0.y+1.)*cfg.height/2.;
            int x1 = (v1.x+1.)*cfg.width/2.;
            int y1 = (v1.y+1.)*cfg.height/2.;
            draw_line(x0, y0, x1, y1, image, c);
        }
    }
    return image;
}

void draw_line(s32 xi, s32 yi, s32 xf, s32 yf, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (abs(xi-xf)<abs(yi-yf))
    {
        swap(xi, yi);
        swap(xf, yf);
        steep = true;
    }
    if (xi>xf) {
        std::swap(xi, xf);
        std::swap(yi, yf);
    }
    s32 dx = xf-xi;
    s32 dy = yf-yi;
    s32 derr2 = abs(dy)*2;
    s32 err2 = 0;
    s32 y = yi;
    if(steep)
    {
        for(int x = xi; x<=xf; ++x)
        {
            image.set(y, x, color);
            err2 += derr2;
            if(err2 > dx)
            {
                y += yf > yi ? 1 : -1;
                err2 -= dx*2;
            }
        }
    }
    else
    {
        for(int x = xi; x<=xf; ++x)
        {
            image.set(x, y, color);
            err2 += derr2;
            if(err2 > dx)
            {
                y += yf>yi? 1 : -1;
                err2 -= dx*2;
            }
        }
    }
}
