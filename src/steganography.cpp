#include "exception.h"
#include <steganography.h>

bool steganography::embedKeys( dataloader& img, const size_t& num_bits,
                               const byte& lsbCount, const string& path )
// embeds RSA private key into an image
// places RSA public key and modulous into file
{
    // try
    // {
    img_original = &img;
    std::ofstream passwordFile;
    bytes privateKey, publicKey, modulous;
    encryption::RSA rsa;
    rsa.num_bits = num_bits;
    rsa.generateKeys( privateKey, publicKey, modulous );

    embedData( privateKey, img, lsbCount );

    passwordFile.open( path, std::ios::out );
    passwordFile << rsa.bytesTocpp_int( publicKey );
    passwordFile << endl;
    passwordFile << rsa.bytesTocpp_int( modulous );
    passwordFile << endl;

    passwordFile.close();
    img_embeded = &img;
    return true;
    // }
    // catch ( const invalidData& e )
    //
    // {
    //     // std::cerr << "Invaid data caught : " << e.what() << std::endl;
    //     throw invalidData( "Invalid data exception thrown", 0 );
    // }
    // catch ( const std::exception& e )
    // {
    //     throw invalidData( "std::exception thrown", 1 );
    // }
    // catch ( ... )
    // {
    //     throw invalidData( "Unknown exception caught", 2 );
    // }
}

bool steganography::extractKeys( bytes& privateKey, bytes& publicKey,
                                 bytes& modulous, dataloader& img,
                                 const size_t& num_bits, const string& path )
{
    // try
    // {
    img_embeded = &img;
    if ( img_embeded == nullptr )
    {
        throw invalidData( "[ERROR] - No image Data!" );
    }
    encryption::RSA rsa;
    rsa.num_bits = num_bits;
    std::ifstream passwordFile;
    string temp = "";

    privateKey = steganography::extractData( img );

    passwordFile.open( path, std::ios::in );
    std::getline( passwordFile, temp );
    publicKey = rsa.cpp_intToBytes( rsa.strToCpp_int( temp ) );
    std::getline( passwordFile, temp );
    modulous = rsa.cpp_intToBytes( rsa.strToCpp_int( temp ) );
    passwordFile.close();
    return true;
    // }
    // catch ( const invalidData& e )
    //
    // {
    //     // std::cerr << "Invaid data caught : " << e.what() << std::endl;
    //     throw invalidData( "Invalid data exception thrown", 0 );
    // }
    // catch ( const std::exception& e )
    // {
    //     throw invalidData( "std::exception thrown", 1 );
    // }
    // catch ( ... )
    // {
    //     throw invalidData( "Unknown exception caught", 2 );
    // }
}

bool steganography::encrypt_pw( const string& pw, dataloader& img,
                                const size_t& num_bits, const string& path )
{
    // try
    // {
    img_embeded = &img;
    encryption::RSA rsa;
    std::ofstream passwordFile;
    bytes privateKey, publicKey, modulous;

    extractKeys( privateKey, publicKey, modulous );
    rsa.setKeys( privateKey, publicKey, modulous );

    passwordFile.open( path, std::ios::app );

    for ( auto c : rsa.encrypt( pw ) )
    {
        passwordFile << c << '\t';
    }
    passwordFile << endl;
    passwordFile.close();

    return true;
    // }
    // catch ( const invalidData& e )
    //
    // {
    //     // std::cerr << "Invaid data caught : " << e.what() << std::endl;
    //     throw invalidData( "Invalid data exception thrown", 0 );
    // }
    // catch ( const std::exception& e )
    // {
    //     throw invalidData( "std::exception thrown", 1 );
    // }
    // catch ( ... )
    // {
    //     throw invalidData( "Unknown exception caught", 2 );
    // }
}

vector<string> steganography::decrypt_pw( dataloader& img,
                                          const size_t& num_bits,
                                          const string& path )
{
    // try
    // {
    img_embeded = &img;
    encryption::RSA rsa;
    std::ifstream passwordFile;
    bytes privateKey, publicKey, modulous;
    string cypher_pw, temp;
    vector<string> passwords;
    vector<bytes> constructed_pw;

    extractKeys( privateKey, publicKey, modulous, img );
    rsa.setKeys( privateKey, publicKey, modulous );

    passwordFile.open( path, std::ios::in );
    std::getline( passwordFile, temp );
    std::getline( passwordFile, temp );

    while ( std::getline( passwordFile, temp ) )
    {
        for ( char c : temp )
        {
            if ( c != '\t' )
            {
                cypher_pw += c;
            }
            else
            {

                constructed_pw.push_back(
                    rsa.cpp_intToBytes( rsa.strToCpp_int( cypher_pw ) ) );
                cypher_pw = "";
            }
        }
        passwords.push_back(
            rsa.decrypt( rsa.bytesVecTocpp_intVec( constructed_pw ) ) );
        constructed_pw.clear();
    }
    passwordFile.close();
    return passwords;
    // }
    // catch ( const invalidData& e )
    //
    // {
    //     // std::cerr << "Invaid data caught : " << e.what() << std::endl;
    //     throw invalidData( "Invalid data exception thrown", 0 );
    // }
    // catch ( const std::exception& e )
    // {
    //     throw invalidData( "std::exception thrown", 1 );
    // }
    // catch ( ... )
    // {
    //     throw invalidData( "Unknown exception caught", 2 );
    // }
}

bool steganography::embedData( vector<byte> bytesToEmbed, dataloader& img,
                               const byte& lsbCount )
{
    // input validation need custom exception class
    if ( bytesToEmbed.empty() )
    {
        // throw imgException()
        // cerr << "Error: Data to Encode empty! " << endl;
        // return false;
        throw invalidData( "[ERROR] - Data to Encode empty", 0 );
    }
    if ( img.imageRaw.empty() )
    {
        // throw imgException()
        // cerr << "Error: Image Data Empty! " << endl;
        // return false;
        throw invalidData( "[ERROR] - Image Data Empty!", 1 );
    }

    if ( bytesToEmbed.size() * 8 > img.imageRaw.size() * lsbCount )
    {
        // throw imgException()
        // cerr << "Error: Image too small to hold data" << endl;
        // return false;
        throw invalidData( "[ERROR] - Image to small to hold data!", 2 );
    }
    auto sub_px = img.imageRaw.begin(); //+ 60
    auto bytes  = bytesToEmbed.begin();

    // need to embed first 1 with 3 bits to show lsbCount
    // this is needed no exceptions padding has to start
    // afterwards if we want padding, still needs added
    *sub_px++ = lsbCount | *sub_px & ~7; // to state lsb

    // left shift 1 by lsbCount subtract by one to fill lsb
    // for the mask its important
    byte lsbMask  = ( 1 << lsbCount ) - 1;
    size_t bitPos = 0;

    /*
    for ( int i = 1; i < 60; i++ )
    // padding example
    {
        img.imageRaw[ i ] = 0;
    }
    */

    int bitsCollected = 0; // counter for bits covered in lsbCount
    byte temp         = 0; // builds bits to mask on sub_px
    while ( sub_px != img.imageRaw.end() )
    {
        *sub_px &= ~lsbMask;
        if ( bytes - 1 != bytesToEmbed.end() )
        {
            for ( int bitPos = 7; bitPos >= 0; --bitPos )
            // from our byte we go from msb to lsb from the data we want to
            // embed we shift and mask it according to lsbCount and store it in
            // temp and mask our current sub pixels lsb/s
            {
                byte currentBit = ( *bytes >> bitPos ) & 1;
                temp            = ( temp << 1 ) | currentBit;
                ++bitsCollected;
                if ( bitsCollected == lsbCount )
                {
                    *sub_px = ( *sub_px & ~lsbMask ) | ( temp & lsbMask );
                    ++sub_px;
                    temp          = 0;
                    bitsCollected = 0;
                }
            }
            ++bytes;
        }
        else if ( bitPos == 0 )
        {
            byte stop_mask = ( 1 << ( lsbCount + 1 ) ) - 1;
            for ( ; bitPos < 64; ++bitPos )
            // we have this to know where our precision starts
            // we lose quite a bit of data but like fuck it we ball
            // without the known padding at the end we have no idea
            // where our shit ends which is problematic
            {
                *sub_px++ = *sub_px & ~stop_mask;
            }
        }
        else
        {
            sub_px++;
        }
    }
    return true;
}

vector<byte> getBits_fromLsb( const vector<byte>& input, int lsbCount )
{
    vector<byte> bit_array;
    const byte mask = ( 1 << lsbCount ) - 1;
    for ( byte b : input )
    {
        byte bits = b & mask;
        for ( int i = lsbCount - 1; i >= 0; --i )
        {
            bit_array.push_back( ( bits >> i ) & 1 );
        }
    }
    return bit_array;
}

vector<byte> steganography::extractData( dataloader& img )
{
    if ( img.imageRaw.empty() || img_embeded == nullptr )
    {
        throw invalidData( "[ERROR] - Image data Empty!", 0 );
    }
    vector<byte> embeddedBitData;
    auto sub_px           = img.imageRaw.end();
    auto bsub_px          = img.imageRaw.begin();
    const byte lsbCount   = *bsub_px++ & 7; // get lsb count (MANDITORY)
    const byte lsbMask    = ( 1 << lsbCount ) - 1;
    const byte start_mask = ( 1 << ( lsbCount + 1 ) ) - 1;
    // std::cout << std::bitset<8>( lsbCount ) << std::endl;
    if ( !lsbCount )
    {
        throw invalidData( "[ERROR] - Least Significant Bit not found!", 1 );
    }

    while ( *bsub_px == 0 )
    // remove padding from the front doing here because should save
    // space I think overall... if imageRaw is 200kb and we have
    // 100kb of padding say we have a lsbCount of 4 we will have
    // a embeddedBitData vector of 400kb of just zeros...
    {
        ++bsub_px;
    }

    int start_steg = 0; // counts masked 1 + lsbCount sub_px bits == 0
    while ( sub_px != bsub_px )
    {
        --sub_px;
        byte currentSub_px = *sub_px & start_mask;
        if ( currentSub_px == 0 )
        {
            ++start_steg;
        }
        else if ( start_steg < 64 )
        {
            start_steg = 0;
        }
        else
        {
            embeddedBitData.assign( bsub_px, sub_px );

            break;
        }
    }
    if ( sub_px == bsub_px )
    {
        throw invalidData( "[ERROR] - Couldn't find embedded data!", 2 );
    }
    // for ( byte& a : embeddedBitData )
    // {
    //     a &= lsbMask;
    // }

    int counter = 0;
    byte temp   = 0;
    vector<byte> byteData;

    for ( byte bitData : getBits_fromLsb( embeddedBitData, lsbCount ) )
    // TODO: Write a function that takes array of bits with LSB bits and
    // returns them as a byte
    //
    // every bit in here will be in lsb so we have to left shift
    // the bits and mask as we go to fill a full byte
    {
        temp |= ( bitData & 1 ) << ( 7 - ( counter % 8 ) );

        ++counter;

        if ( counter % 8 == 0 )
        {
            byteData.push_back( temp );
            temp = 0;
        }
    }
    // last bit has to be valid because we lose precision when dealing with
    // large numbers!
    // byteData.push_back( temp );

    return byteData;
}
