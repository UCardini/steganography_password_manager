#include "dataloader.h" 
#include "encryption.h" 
#include "exception.h" 
#include "steganography.h" 
/**/
using std::string, std::cout, std::endl, std::vector; 
using namespace encryption;
int main( int argc, char* argv[] )
{ 
  string inFile  = "data/input/image.png"; 
  string outFile = "data/output/image.png";

    vector<string> passwords = { "123456789", "Password", "Dragon", "Password123", "Password123!" }; 
  dataloader img;

  try 
  { 
    img.loadImage( inFile ); 
    steganography::embedKeys( img ); 
    img.saveImage( outFile ); 
    for ( string pw : passwords ) 
    { 
      steganography::encrypt_pw( pw ); 
    } 

    img.saveImage( outFile ); 

    for ( string pw : steganography::decrypt_pw( img ) ) 
    cout << pw << endl; 
  } 
  catch ( const invalidData& e ) 
  { 
    std::cerr << e.what() << endl; 


  }
    return 0; 
}
