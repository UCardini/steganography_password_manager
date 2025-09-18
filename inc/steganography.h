#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include "dataloader.h"
#include "encryption.h"
#include "exception.h"

using std::vector, std::cerr, std::endl, std::string;
using byte  = unsigned char;
using bytes = vector<byte>;

class steganography : dataloader
// LSB steganography
{
public:
    static bool embedKeys( dataloader& img        = *img_original,
                           const size_t& num_bits = 64,
                           const byte& lsbCount   = 1,
                           const string& path = "data/output/passwords.csv" );

    static bool extractKeys( bytes&, bytes&, bytes&,
                             dataloader& img        = *img_embeded,
                             const size_t& num_bits = 64,
                             const string& path = "data/output/passwords.csv" );

    static bool encrypt_pw( const string& pw, dataloader& img = *img_embeded,
                            const size_t& num_bits = 64,
                            const string& path = "data/output/passwords.csv" );

    static vector<string>
    decrypt_pw( dataloader& img = *img_embeded, const size_t& num_bits = 64,
                const string& path = "data/output/passwords.csv" );

private:
    inline static dataloader* img_original = nullptr;
    inline static dataloader* img_embeded  = nullptr;
    /* ***embedData***
     * This takes raw bytes from image data and raw bytes to embed into
     * the image data. lsbCount is the number of bits at the end of the image
     * bytes you want to embed your data in the resulting image data for larger
     * lsbCount's will be lossy but with the trade off of storing more data
     *
     * The first sub pixel of the image data contains the lsbCount, following
     * is the rest of the embedded data. After the embedded data an arbitrary
     * amount of bytes masked #(lsbCount + 1) right bits == 0's to hopefully
     * decrease the odds of running into problems and with the goal of
     * maintaining precision.
     *
     *
     * TODO: I overwrote extractData to allow padding to be inserted between
     * first embedded byte containing lsbCount and the rest of the data.
     * Theres a potential that being able to add padding might be useful.
     */
    static bool embedData( bytes, dataloader&, const byte& lsbCount = 4 );

    /* ***extractData***
     * This takes raw bytes from image data embedded using embedData reads
     * the first byte to get the lsbCount ignores #lsbCount right bits == 0
     * until first masked byte contains data. Data is then read masked and
     * returned as a vector of unsigned chars... Precision is known by counting
     * an arbitrary amount of bytes masked by #(lsbCount+1) right bits == 0.
     * When the arbitrary amount (64) is met embedded data is found break loop
     * for further processing.
     */
    static bytes extractData( dataloader& img );
};

#endif // !STEGANOGRAPHY_H
