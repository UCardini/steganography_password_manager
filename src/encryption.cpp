#include "encryption.h"
#include <boost/multiprecision/cpp_int/import_export.hpp>
using namespace encryption;

template <typename T> vector<T> RSA<T>::encrypt( const string& password )
{
    if ( keys.modulous == 0 )
    {
        generateKeys();
    }

    std::vector<cpp_int> cypherArr;
    for ( char c : password )
    {
        if ( keys.modulous - 1 < cpp_int( c ) )
        {

            std::cerr << "Need larger number of bits to encrypt: " << password
                      << std::endl;
            return cypherArr;
        }
        cypherArr.push_back(
            modExp( cpp_int( c ), keys.publicKey, keys.modulous ) );
    }
    return cypherArr;
}

template <typename T> string RSA<T>::decrypt( const vector<T>& cypherArr )
{

    if ( keys.modulous == 0 )
    {
        generateKeys();
    }

    std::string decryptedMessage = "";
    for ( size_t i = 0; i < cypherArr.size(); i++ )
    {
        cpp_int decrypted =
            modExp( cypherArr[ i ], keys.privateKey, keys.modulous );
        char c = static_cast<char>( decrypted );
        decryptedMessage += c;
    }
    return decryptedMessage;
}

template <typename T> void RSA<T>::generateKeys() // const
{
    if ( num_bits < 4 )
    {
        std::cerr << "Need at least 4 bits to compute key pair!" << std::endl;
        num_bits = 4;
        // return;
    }

    T p = generatePrime();
    T q = generatePrime();
    while ( p == q )
    // p and q must be different
    {
        q = generatePrime();
    }
    keys.modulous = p * q;
    T phi         = ( p - 1 ) * ( q - 1 );

    keys.publicKey = 65537; // known public exponent

    while ( gcd( keys.publicKey, phi ) != 1 )
    // public exponent has to be coprime with phi
    {
        keys.publicKey++;
        std::cerr << "Key changed!" << std::endl;
    }
    keys.privateKey = modInverse<T>( keys.publicKey, phi );
}

template <typename T> const T RSA<T>::generatePrime() // const
{
    // declare generator to generate random bits
    // Doing this in here for now realistically should be in encrypt funct
    independent_bits_engine<boost::mt19937, this->MAX_BITS, T> gen( time( 0 ) );

    T raw = gen(); // ensures odd
    raw |= 1;
    T mask = ( cpp_int( 1 ) << this->num_bits ) - 1;
    T n = raw & mask; // sets MAX_BITS number of bits and masks num_bits from it

    // this should be left
    while ( !isPrime( n ) )
    {
        n += 2;
    }
    return n;
}

template <typename T> bool RSA<T>::isPrime( const T& n ) const
// up to general prime check should eliminate most candidates
// hopefully to speed up prime check
{
    /*
     * small num checks
     */
    if ( n <= 1 )
    // check if n is 1 or 0
    {
        return false;
    }
    if ( n == 2 || n == 3 )
    // check if n is 2 or 3
    {
        return true;
    }

    /*
     * sweeping check to elimate candidates quickly
     */
    if ( n % 2 == 0 || n % 3 == 0 )
    // Check whether n is divisible by 2 or 3
    {
        return false;
    }

    T temp = n;
    T sum  = 0; // sum of the digits

    while ( temp != 0 )
    // get sum of all the digits in n
    {
        sum += temp % 10;
        temp /= 10;
    }

    if ( sum % 3 == 0 )
    // check divisability by 3 for the sum of the digits if true cannot be prime
    {
        return false;
    }

    /*
     * general prime check can be using various methods
     */
    if ( 0 < this->num_bits )
    // need to change this to actually do something...
    {
        return boost::multiprecision::miller_rabin_test( n, 25 );
        // return primeTest::probabilistic::millerRabin( n );
    }
    else
    {
        return primeTest::deterministic::trialDivision( n );
    }
}

template <typename T> vector<byte> RSA<T>::cpp_intToBytes( T n )
{
    vector<byte> bytes;
    export_bits( n, back_inserter( bytes ), 8 );
    return bytes;
}

template <typename T> T RSA<T>::bytesTocpp_int( vector<byte> n )
{
    T intOutput;
    import_bits( intOutput, n.begin(), n.end(), 8 );
    return intOutput;
}

template <typename T>
void RSA<T>::generateKeys( vector<byte>& privateKey, vector<byte>& publicKey,
                           vector<byte>& modulous )
{
    generateKeys();
    privateKey = cpp_intToBytes( this->keys.privateKey );
    publicKey  = cpp_intToBytes( this->keys.publicKey );
    modulous   = cpp_intToBytes( this->keys.modulous );
}

template <typename T>
void RSA<T>::setKeys( vector<byte>& privateKey, vector<byte>& publicKey,
                      vector<byte>& modulous )
{
    this->keys.privateKey = bytesTocpp_int( privateKey );
    this->keys.publicKey  = bytesTocpp_int( publicKey );
    this->keys.modulous   = bytesTocpp_int( modulous );
}

template <typename T> cpp_int RSA<T>::strToCpp_int( string myStr )
{
    cpp_int p( myStr );
    return p;
}

template <typename T>
vector<cpp_int> RSA<T>::bytesVecTocpp_intVec( vector<vector<byte>>& n )
{
    vector<cpp_int> p;
    for ( auto c : n )
    {
        p.push_back( bytesTocpp_int( c ) );
    }
    return p;
}
