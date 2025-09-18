#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include "mathHelpers.h"

using namespace boost::multiprecision;
using namespace boost::random;
using namespace mathHelpers;
using std::string, std::vector;
using byte = unsigned char;

namespace encryption
{

template <typename T = cpp_int> class RSA
{
    inline const static size_t MAX_BITS = 1024; // must be set at compile time

public:
    size_t num_bits = 64; // default size of 64 bits

    struct keyPair
    {
        T publicKey;  // e
        T privateKey; // d
        T modulous;   // n
    };

    keyPair keys;

    RSA()  = default;
    ~RSA() = default;

    void generateKeys();
    void generateKeys( vector<byte>&, vector<byte>&, vector<byte>& );

private:
    const T generatePrime(); // const;
    bool isPrime( const T& n ) const;

public: // User Fuctions
    vector<T> encrypt( const string& );
    string decrypt( const vector<T>& );
    static vector<byte> cpp_intToBytes( T n );
    static T bytesTocpp_int( vector<byte> n );
    void setKeys( vector<byte>& privateKey, vector<byte>& publicKey,
                  vector<byte>& modulous );
    static cpp_int strToCpp_int( string );
    static vector<cpp_int> bytesVecTocpp_intVec( vector<vector<byte>>& n );
};

} // namespace encryption

template class encryption::RSA<cpp_int>;
#endif // !ENCRYPTION_H
