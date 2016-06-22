#ifndef FRAME_H
#define FRAME_H


#include <string>

using std::string;


#define ACK     0x06
#define STX     0x02
#define ETX     0x03
#define DLE     0x10
#define NULO    0x00
#define NACK    0x15

/**
 * @enum WaitState
 * Reception status enum.
 */
enum WaitState{
    WAIT_DLE=0,
    WAIT_STX,
    WAIT_DATA,
    WAIT_NEXT_DLE,
    WAIT_ETX,
    WAIT_EXTRA
};


/**
    @brief Decoding and encoding frames Class
    
    This class decodes and encodes ASCII frames of the form (DLE STX  PAYLOAD DLE ETX EXTRABYTES).
    
	Use it by instantiation a Frame object, and add data using addByte until you have received a full
	frame.  You must always check addByte return value in order to know if you have already received a frame or
	next time you use addbye received frame will be discarded.
	
	Usage example
	@code
        Frame frame(2, 1);  // We are going to process two-byte frames and 1 extra byte
        do {
		    char byte = popNextByteFromBuffer();
		} while ( !frame.addByte(byte) );
		
		cout << "Received data: << frame[0] << frame[1] << " Extra Data: " << frame.getExtraData().at(0);
	@endcode
*/
class Frame{
public:
    Frame(int fixdatalen = 0, int extrabytes = 0);
    ~Frame();

    bool addByte(char byte);
    void setFixDataLen(const int& v){ _fixdataLen = v; }
    int getFixDataLen() const{ return _fixdataLen; }
    void setExtraBytes(const int& v){ _extra = v; }
    int getExtraBytes() const{ return _extrabytes; }
    unsigned int size(){ return _data.size(); }

    const char & operator[](unsigned int i) const;
    char & operator[](unsigned int i);

    string getData() const { return _data; }
    string getExtraData() const { return _extra; }

    void clear() {
        _state = WAIT_DLE; _data.clear(); _extra.clear();
    }

    static string buildFrame(string data, bool duplicateDLE=true);


protected:
    unsigned int _fixdataLen;
    unsigned int _extrabytes;
    WaitState _state;
    string _data;
    string _extra;

};

#endif
