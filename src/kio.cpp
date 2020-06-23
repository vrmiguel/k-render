#include "includes/kio.h"
#include <string.h>
#include <fstream>

using std::cout;
using std::cerr;
using std::ofstream;

//! kio: Input/output module for krender

config_t parse_cli_input(int argc, char ** argv)
{
    config_t cfg;
    if (argc == 1)
    {
        cerr << "Usage: ./krender [-w, --width <width>] [-r, --rotate <theta>] [-h, --height <height>] -o, --obj <obj-file>\n";
        cerr << "Use options '-H' or '--help' for help.\n";
        exit(0);
    }
    bool width_set = false, height_set = false, obj_set;
    for (u8 i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--rotate"))
        {
            if (i + 1 >= argc)
            {
                cerr << "krender: missing value to --rotate";
                exit(0);
            }
            cfg.rotation_set = true;
            cfg.rotation = std::stof(argv[++i]);
        }
        else if (!strcmp(argv[i], "-H") || !strcmp(argv[i], "--help"))
        {
            printf("krender beta - github.com/vrmiguel/krender\n");
            printf("%-20s\tSets the output TGA's width.  Optional.\n", "-w, --width <arg>");
            printf("%-20s\tSets the output TGA's height. Optional.\n", "-h, --height <arg>");
            printf("%-20s\tSets the .OBJ file to be loaded.\n","-o, --o <obj>");
            printf("%-20s\tShows this message and exits.\n",        "-H, --help");
        }
        else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--width"))
        {
            if (i + 1 >= argc)
            {
                cerr << "krender: missing value to --width";
                exit(0);
            }
            cfg.width = std::atoi(argv[++i]);
            width_set = true;
        }
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--height"))
        {
            if (i + 1 >= argc)
            {
                cerr << "krender: missing value to --height";
                exit(0);
            }
            cfg.height = std::atoi(argv[++i]);
            height_set = true;
        }
        else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--obj"))
        {
            if (i + 1 >= argc)
            {
                cerr << "krender: missing value to --obj";
                exit(0);
            }
            cfg.obj_file = argv[++i];
            obj_set = true;
        }
        else {
            cerr << "krender: unknown option \"" << argv[i] << "\".\n";
        }
    }

    if (!obj_set)
    {
        cerr << "krender: fatal: no Waveform .obj file supplied. Exiting.\n";
        exit(0);
    }

    if (height_set && !width_set)
    {
        cerr << "krender: assuming width = " << cfg.height << ".\n";
        cfg.width = cfg.height;
    }

    else if (!height_set && width_set)
    {
        cerr << "krender: assuming height = " << cfg.width << ".\n";
        cfg.height = cfg.width;
    }

    else if (!height_set && !width_set)
    {
        cerr << "krender: height and width not set. Assuming 3200 x 3200.\n";
        cfg.height = cfg.width = 3200;
    }
    return cfg;
}

//! Heavily based off of Dmitry V. Sokolov's TGA saving code.
//! See LICENSE.md or ktypes.h/.cpp for Dmitry's copyright notice.
bool save_result(TGAImage img, const char *filename, bool rle) {
    u8 developer_area_ref[4] = {0, 0, 0, 0};
    u8 extension_area_ref[4] = {0, 0, 0, 0};
    u8 footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
    ofstream out;
    img.flip_vertically();
    out.open (filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        out.close();
        return false;
    }
    TGA_Header header;
    memset((void *)&header, 0, sizeof(header));
    header.bitsperpixel = img.get_bytespp() <<3;
    header.width  = img.get_width();
    header.height = img.get_height();
    header.datatypecode = (img.get_bytespp() ==GRAYSCALE?(rle?11:3):(rle?10:2));
    header.imagedescriptor = 0x20;
    out.write((char *)&header, sizeof(header));
    if (!out.good()) {
        out.close();
        std::cerr << "krender: error: could't dump TGA file.\n";
        return false;
    }
    if (!rle) {
        out.write((char *) img.buffer(), img.get_width()*img.get_height()*img.get_bytespp());
        if (!out.good()) {
            std::cerr << "krender: error: could't unload RAW data.\n";
            out.close();
            return false;
        }
    } else {
        if (!img.unload_rle_data(out)) {
            out.close();
            std::cerr << "krender: error: could't unload RLE data.\n";
            return false;
        }
    }
    out.write((char *)developer_area_ref, sizeof(developer_area_ref));
    if (!out.good()) {
        std::cerr << "krender: error: could't dump TGA file.\n";
        out.close();
        return false;
    }
    out.write((char *)extension_area_ref, sizeof(extension_area_ref));
    if (!out.good()) {
        std::cerr << "krender: error: could't dump TGA file.\n";
        out.close();
        return false;
    }
    out.write((char *)footer, sizeof(footer));
    if (!out.good()) {
        std::cerr << "krender: error: could't dump TGA file.\n";
        out.close();
        return false;
    }
    out.close();
    cout << "krender: successfully saved \"" << filename << "\".\n";
    return true;
}
