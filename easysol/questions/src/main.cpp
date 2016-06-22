#include <deque>
#include <cstdlib>

#include "frame.h"

/** Incomming bytes emulation buffer */
std::deque<char> incommingBuffer;

void simIncommingBytes();

int main(int argc, char *argv[])
{
   //Here your user sequence input code.

   srand(1);
   while ( true ){
      simIncommingBytes();
      //Here your incomming frames processing function.
   }
}

/**
 * Simulates incomming bytes. Generates pseudorandom byte sequences and frames 
 * and puts it into incommingBuffer.
 * This function could be optimized but such effort is not required for this test.
 */
void simIncommingBytes()
{
   int i = rand()%10;
   if ( i < 5 ) {
      string data;

      data += static_cast<char>(i);
      data = Frame::buildFrame(data);

      incommingBuffer.insert( incommingBuffer.end(), data.begin(), data.end() );

      //cout <<  i ;
   } else {
      int rndDtaCnt = rand()%10;
      for ( i = 0; i<rndDtaCnt; ++i ){
         incommingBuffer.push_back( static_cast<char>(rand()%10) );
      }
   }
}

