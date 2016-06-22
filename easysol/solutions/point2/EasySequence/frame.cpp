#include "frame.h"



/*! \fn void Frame::clear()
 * Resets reception buffers and flags. All received bytes will be lost.
 */

/*! \fn string Frame::getData() const
 * Returns data recived.  It does not include control characters. 
 */
    
/*! \fn string Frame::getExtraData() const
 * Returns extra bytes received
 */

/*! \fn void Frame::setFixDataLen(const int& v)
 * Sets a fixed frame data length.
 * @param v  Data length or 0 if for variable length data.
 */

/*! \fn int Frame::getFixDataLen() const
 * Returns the value set by setFixDataLen or by the constructor.
 */

/*! \fn void Frame::setExtraBytes(const int& v)
 * Sets the extra-bytes to receive after the full frame. It is used for CRC additional data. Set to 0 if you
 * wont receive extra-bytes.
 */

/*! \fn int Frame::getExtraBytes() const
  *  Returns extra bytes recieved
  */

/*! \fn unsigned int Frame::size()
 * Returns the size of data received.
 */


/**
 * Creates a Frame with fixdatalen data length and extrabytes extra bytes
 * @param fixdatalen Fixed length for frames o 0 if variable length
 * @param extrabytes Extra bytes to expect after a full frame recieved. Useful for CRC after a frame.
 */
Frame::Frame(int fixdatalen, int extrabytes)
{
    _fixdataLen = fixdatalen;
    _extrabytes = extrabytes;
    _state = WAIT_DLE;
    _data.clear();
    _extra.clear();
}


Frame::~Frame()
{
}




/*!
	Insert received data byte into frame buffer. Returns true when a full frame has been received.
        \param[in] byte Byte to add
 */
bool Frame::addByte(char byte)
{
    bool retval = false;

    if( (_fixdataLen!=0 && (_data.size() > (_fixdataLen+1))) || _data.size()>=100){
        _data.clear();
        _state = WAIT_DLE;
    }

    switch(_state){
        case WAIT_DLE:
            if( byte != DLE ){
                retval = false;
            }else{
                _state=WAIT_STX;
                retval = false;
            }
            break;

        case WAIT_STX:
            if( byte != STX ){
                _state = WAIT_DLE;
                retval = false;
            }else{
                _state = WAIT_DATA;
                _data.clear();
                retval = false;
            }
            break;

        case WAIT_DATA:
            _data += byte;
            if( byte == DLE ){
                if(_fixdataLen == 0){
                    _state = WAIT_NEXT_DLE;
                }else if((_data.size()-1) == _fixdataLen){
                    _state = WAIT_ETX;
                }
            }else{
                _state = WAIT_DATA;
            }
            retval = false;

            break;

        case WAIT_NEXT_DLE:
            if( byte == DLE ){
                _state = WAIT_DATA;
                retval = false;
            }else{
                if( byte == ETX){
                    _data.resize( _data.size()-1 );
                    if(_extrabytes > 0){
                        _extra.clear();
                        _state = WAIT_EXTRA;
                        retval = false;
                    }else{
                        _state = WAIT_DLE;
                        retval = true;
                    }
                }else{
                    _state = WAIT_DLE;
                    retval = false;
                }
            }
            break;

        case WAIT_ETX:
            if( byte == ETX){
                _data.resize( _data.size()-1 );
                if(_extrabytes > 0){
                    _state = WAIT_EXTRA;
                    _extra.clear();
                    retval = false;
                }else{
                    _state = WAIT_DLE;
                    retval = true;
                }

            }else{
                _state = WAIT_DLE;
                retval = false;
            }
            break;

        case WAIT_EXTRA:
            _extra += byte;
            if(_extra.size() == _extrabytes){
                _extra.resize( _extrabytes );
                _state = WAIT_DLE;
                retval = true;
            }

            break;

    }

    return retval;
}


/**
 * Overloaded operator used to get individual bytes from frame data.
 * Use with caution. Accessing unexisting index will produce unknown behavior.
 * @param i byte index
 */
const char & Frame::operator[](unsigned int i) const
{
    return _data[i];
}


char & Frame::operator[](unsigned int i)
{
    return _data[i];
}



/*!
    Builds a frame with data.
    \param[in] data Payload
    \param[in] duplicateDLE true if you want to duplicate DLE characters found in payload. 
	Usually you don't duplicate DLE if you are using fixed frame length.
 */
string Frame::buildFrame(string data, bool duplicateDLE)
{
    string tmp;
    unsigned int i;

    tmp.clear();
    tmp += static_cast<char>(DLE);
    tmp += static_cast<char>(STX);
    //Ajuste para DLE existentes dentro de los datos
    for(i=0; i< data.size(); i++){
        tmp += data[i];
        if(data[i]==DLE && duplicateDLE){
            tmp += data[i];
        }
    }

    tmp += static_cast<char>(DLE);
    tmp += static_cast<char>(ETX);

    return tmp;
}
