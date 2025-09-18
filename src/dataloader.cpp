#include "dataloader.h"

bool dataloader::loadImage( const std::string& path )
{
    unsigned error = lodepng::decode( imageRaw, width, height, path );
    if ( error )
        std::cerr << "decoder error " << error << ": "
                  << lodepng_error_text( error ) << std::endl;
    return ( !error );
    // throw decoderError();
}

bool dataloader::saveImage( const std::string& path )
{
    unsigned error =
        lodepng::encode( path, imageRaw, this->width, this->height );
    if ( error )
        std::cout << "encoder error " << error << ": "
                  << lodepng_error_text( error ) << std::endl;
    return !error;
}
