#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random.hpp>

#include <openssl/sha.h>
#include <random>
#include <string>

#include <iostream>
#include <vector>

#ifndef MATHHELPERS_H
#define MATHHELPERS_H

using namespace boost::multiprecision;
using std::string;

namespace mathHelpers
{

template <typename T1 = cpp_int, typename T2 = cpp_int>
T2 modExp( const T1& x, const T2& y, const T2& m )
// To compute x^y under modulo m
{
    if ( y == 0 )
    {
        return 1;
    }
    T2 p = modExp<T1, T2>( x, y / 2, m ) % m;
    p    = ( p * p ) % m;

    if ( y % 2 == 0 )
    {
        return p;
    }
    return ( x * p ) % m;
}

template <typename T> T gcd( const T& a, const T& b )
// Function to return gcd of a and b
{
    if ( a == 0 )
    {
        return b;
    }
    return gcd( b % a, a );
}

template <typename T> T modInverse( T a, T m )
{
    T m0 = m, t, q;
    T x0 = 0, x1 = 1;

    if ( m == 1 )
        return 0;

    while ( a > 1 )
    {
        q = a / m;
        t = m;
        m = a % m, a = t;
        t  = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if ( x1 < 0 )
        x1 += m0;

    return x1;
}

// template <typename T> T modInverse( const T& a, const T& m )
//
// {
//     T g = gcd( a, m );
//     // return g;
//     if ( g != 1 )
//     {
//         std::cerr << "Inverse doesn't exist";
//         return 0;
//         // throw nonexistantInverse();
//     }
//     else
//     {
//         // If a and m are relatively prime, then modulo inverse
//         // is a^(m-2) mod m
//         return modExp<T, T>( a, m - 2, m );
//     }
// }

namespace primeTest
{
namespace probabilistic
{

template <typename T> bool millerRabin( const T& n, const int& k = 20 )
// assuming that n > 2 and is an odd integer
// k = # of rounds for the test
{
    // find largest (power of 2 that divides n - 1)
    // AE: n-1 = 2^r * d, where d is odd
    T d   = n - 1;
    int s = 0;
    while ( d % 2 == 0 )
    {
        d /= 2;
        ++s;
    }

    boost::random::mt19937 gen( std::random_device{}() );
    boost::random::uniform_int_distribution<T> dist( 2, n - 2 );
    for ( int i = 0; i < k; i++ )
    {
        T a = dist( gen );
        T x = boost::multiprecision::powm( a, d, n );

        for ( int r = 0; r < s; ++r )
        {
            T y = boost::multiprecision::powm( x, 2, n );
            if ( y == 1 && x != 1 && x != n - 1 )
            {
                return false;
            }
            x = y;
        }

        if ( x != 1 )
        {
            return false;
        }
    }
    return true;
}

} // namespace probabilistic

namespace deterministic
{
template <typename T>

T integerSqrt( const T& n )
{
    using mFloat  = number<cpp_dec_float<100>>;
    mFloat approx = sqrt( mFloat( n ) );
    return T( approx ); // return as T should be cpp_int
}

template <typename T> bool trialDivision( const T& n )
{
    if ( n <= 1 )
        return false;
    if ( n <= 3 )
        return true;
    if ( n % 2 == 0 )
        return false;

    T limit = integerSqrt( n );
    for ( T i = 3; i <= limit; i += 2 )
    {
        if ( n % i == 0 )
            return false;
    }

    return true;
}

} // namespace deterministic

} // namespace primeTest
} // namespace mathHelpers
#endif // !MATHHELPERS_H
