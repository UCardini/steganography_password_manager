#include "encryption.h"
#include "lodepng.h"
#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifndef DATALOADER_H
#define DATALOADER_H

using byte = unsigned char;

struct dataloader
{
    unsigned width, height;
    std::vector<byte> imageRaw;
    bool loadImage( const std::string& );
    bool saveImage( const std::string& );
};

#endif // !DATALOADER_H
