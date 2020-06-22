/*!
 * All TGA-related implementations are based off of Dmitry Sokolov's tgaimage.h and tgaimage.cpp, located at
 * https://github.com/ssloy/tinyrenderer/tree/909fe20934ba5334144d2c748805690a1fa4c89f
 *
 * Copyright Dmitry V. Sokolov
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it freely,
 * subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 *
 * Copyright (c) 2020 Vinícius R. Miguel <vinicius.miguel at unifesp.br>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */


#ifndef __KRENDER_KTYPES_H
#define __KRENDER_KTYPES_H

typedef unsigned char u8;
typedef signed char   s8;
typedef unsigned int  u32;
typedef signed int    s32;

struct config_s {
    char * obj_file;
    u32 height;
    u32 width;
};
typedef struct config_s config_t;

enum ColorMode {
    GRAYSCALE=1, RGB=3, RGBA=4
};

#include <fstream>
#include "ktypes.h"

#pragma pack(push,1)
struct TGA_Header {
    s8    idlength;
    s8    colormaptype;
    s8    datatypecode;
    short colormaporigin;
    short colormaplength;
    char  colormapdepth;
    short x_origin;
    short y_origin;
    short width;
    short height;
    s8    bitsperpixel;
    s8    imagedescriptor;
};
#pragma pack(pop)



struct TGAColor {
    union
    {
        struct
        {
            u8 b, g, r, a;
        };
        u8 raw[4];
        u32 val;
    };
    int bytespp;
    TGAColor() : val(0), bytespp(1) { }
    TGAColor(u8 R, u8 G, u8 B, u8 A) : b(B), g(G), r(R), a(A), bytespp(4) { }
    TGAColor(int v, int bpp) : val(v), bytespp(bpp) { }
    TGAColor(const TGAColor &c) : val(c.val), bytespp(c.bytespp) { }

    TGAColor(const u8 *p, int bpp) : val(0), bytespp(bpp) {
        for (int i=0; i<bpp; i++) {
            raw[i] = p[i];
        }
    }

    TGAColor & operator =(const TGAColor &c)
    {
        if (this != &c) {
            bytespp = c.bytespp;
            val = c.val;
        }
        return *this;
    }
};


class TGAImage {
protected:
    u8* data;
    int width;
    int height;
    int bytespp;

public:
    bool   load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out);
    TGAImage();
    TGAImage(int w, int h, int bpp);
    TGAImage(const TGAImage &img);
    bool read_tga_file(const char *filename);
    bool flip_horizontally();
    bool flip_vertically();
    bool scale(int w, int h);
    TGAColor get(int x, int y);
    bool set(int x, int y, TGAColor c);
    ~TGAImage();
    TGAImage & operator =(const TGAImage &img);
    int get_width();
    int get_height();
    int get_bytespp();
    u8 *buffer();
    void clear();
};

#endif

