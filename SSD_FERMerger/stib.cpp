#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

#include "stib.h"

using namespace std;
using namespace stib;

const char *Stib::_opname[] = { "undefined", "write", "set", "ack", "read", "reset", "default", "waitclr", "waitset" };

void Stib::MaskBits(unsigned int *mask,unsigned char how,int chan) {
  int iword = 3-chan/32;
  int ibit = 31-(chan%32);
  if ( how == SET ) {
    mask[iword] |= (1<<ibit);
  }
  else if ( how == CLEAR ) {
    mask[iword] &= ~(1<<ibit);
  }
}

void Stib::MaskBits(unsigned int *mask,unsigned char how) {
  if ( how == SET ) {
    mask[0] = 0xffffffff;
    mask[1] = 0xffffffff;
    mask[2] = 0xffffffff;
    mask[3] = 0xffffffff;
  }
  else if ( how == CLEAR ) {
    mask[0] = 0;
    mask[1] = 0;
    mask[2] = 0;
    mask[3] = 0;
  }
}

//  Warning... no range checking...

unsigned char Stib::StripNumber(unsigned char chan) {
  static unsigned char strip_number[] = { 5, 7, 6, 14, 10, 11, 9, 13 };
  return strip_number[chan%8];
}

unsigned char Stib::SetNumber(unsigned char chan) {
  static unsigned char set_number[] = { 10, 11, 15, 14, 12, 13, 29, 28,
                                        20, 21, 23, 22, 18, 19, 27, 26 };
  return set_number[chan/8];
}

unsigned int Stib::SensorStrip(int chip,int set,int strip) {
static unsigned char set_number[] = { 255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255,   0,   1,   4,   5,   3,   2,
                                      255, 255,  12,  13,   8,   9,  11,  10,
                                      255, 255,  15,  14,   7,   6, 255, 255  };
  static unsigned char strip_number[] = { 255, 255, 255, 255,
                                          255,   0,   2,   1,
                                          255,   6,   4,   5,
                                          255,   7,   3, 255  };

  return 128*(chip-1)+set_number[set]*8+strip_number[strip];
}

Stib::Stib(int s,struct sockaddr *daddr,int dlen) : _socket(s), _daddr(daddr), _dlen(dlen), _pbuf(_txbuf) {
  _id = 0;
  _iseq = 1;
  *_txbuf = 0x00;
}

void Stib::Clear() {
  _id += 1;
  _iseq = 1;
  *_txbuf = 0x00;
  _pbuf = _txbuf;
}

int Stib::Write(unsigned int addr,unsigned int data) {
  *_pbuf++ = _iseq;
  *_pbuf++ = WRITE;
  *_pbuf++ = 0;
  *_pbuf++ = 8;
  *_pbuf++ = (addr>>24)&0xff;
  *_pbuf++ = (addr>>16)&0xff;
  *_pbuf++ = (addr>>8)&0xff;
  *_pbuf++ = addr&0xff;
  *_pbuf++ = (data>>24)&0xff;
  *_pbuf++ = (data>>16)&0xff;
  *_pbuf++ = (data>>8)&0xff;
  *_pbuf++ = data&0xff;
  *_pbuf = 0x00;
  return _iseq++;
}

int Stib::WaitSet(unsigned int addr,unsigned int data,unsigned int timeout) {
  *_pbuf++ = _iseq;
  *_pbuf++ = WAITSET;
  *_pbuf++ = 0;
  *_pbuf++ = 12;
  *_pbuf++ = (addr>>24)&0xff;
  *_pbuf++ = (addr>>16)&0xff;
  *_pbuf++ = (addr>>8)&0xff;
  *_pbuf++ = addr&0xff;
  *_pbuf++ = (data>>24)&0xff;
  *_pbuf++ = (data>>16)&0xff;
  *_pbuf++ = (data>>8)&0xff;
  *_pbuf++ = data&0xff;
  *_pbuf++ = (timeout>>24)&0xff;
  *_pbuf++ = (timeout>>16)&0xff;
  *_pbuf++ = (timeout>>8)&0xff;
  *_pbuf++ = timeout&0xff;
  *_pbuf = 0x00;
  return _iseq++;
}

int Stib::WaitClear(unsigned int addr,unsigned int data,unsigned int timeout) {
  *_pbuf++ = _iseq;
  *_pbuf++ = WAITCLR;
  *_pbuf++ = 0;
  *_pbuf++ = 12;
  *_pbuf++ = (addr>>24)&0xff;
  *_pbuf++ = (addr>>16)&0xff;
  *_pbuf++ = (addr>>8)&0xff;
  *_pbuf++ = addr&0xff;
  *_pbuf++ = (data>>24)&0xff;
  *_pbuf++ = (data>>16)&0xff;
  *_pbuf++ = (data>>8)&0xff;
  *_pbuf++ = data&0xff;
  *_pbuf++ = (timeout>>24)&0xff;
  *_pbuf++ = (timeout>>16)&0xff;
  *_pbuf++ = (timeout>>8)&0xff;
  *_pbuf++ = timeout&0xff;
  *_pbuf = 0x00;
  return _iseq++;
}

int Stib::Read(unsigned int addr) {
  *_pbuf++ = _iseq;
  *_pbuf++ = READ;
  *_pbuf++ = 0;
  *_pbuf++ = 4;
  *_pbuf++ = (addr>>24)&0xff;
  *_pbuf++ = (addr>>16)&0xff;
  *_pbuf++ = (addr>>8)&0xff;
  *_pbuf++ = addr&0xff;
  *_pbuf = 0x00;
  return _iseq++;
}

int Stib::SlowControls(unsigned char chipid,unsigned char mask,unsigned char addr,unsigned char inst) {
  unsigned int w;
  int ierr;
  int len = 0;
  if ( addr < 16 || addr == 27 || addr == 30 ) {
    len = 3;
  }
  else if ( addr == 17 || addr == 18 ) {
    len = 4;
  }
  else if ( addr == 16 ) {
    len = 2;
  }
  else if ( addr == 19 || addr == 20 ) {
    len = 1;
  }
  if ( inst == READ ) {
    w = 0x80000000 | (len<<24) | (1<<(16+mask)) | (mask<<13) | (inst<<10) | (addr<<5) | chipid;
  }
  else {
    w = 0x80000000 | (len<<24) | (mask<<16) | (inst<<10) | (addr<<5) | chipid;
  }
  WaitClear(STRIP_SC_CSR,0x80000000);
  ierr = Write(STRIP_SC_CSR,w);
  WaitClear(STRIP_SC_CSR,0x80000000);
  if ( inst == READ ) {
    return Read(STRIP_SCO);
  }
  return ierr;
}

int Stib::SlowControls(unsigned char chipid,unsigned char mask,unsigned char addr,unsigned char inst,unsigned char data) {
  unsigned int w;
  unsigned char len = 0;
  int iret;

  if ( addr < 16 || addr == 27 || addr == 30 ) {
    len = 3;
  }
  else if ( addr == 17 || addr == 18 ) {
    len = 4;
  }
  else if ( addr == 16 ) {
    len = 2;
  }
  else if ( addr == 19 || addr == 20 ) {
    len = 1;
  }

  WaitClear(STRIP_SC_CSR,0x80000000);
  w = data;
  Write(STRIP_SCI0,w);
  w = 0x80000000 | (len<<24) | (mask<<16) | (inst<<10) | (addr<<5) | chipid;
  iret = Write(STRIP_SC_CSR,w);
  WaitClear(STRIP_SC_CSR,0x80000000);
  return iret;
}

int Stib::SlowControls(unsigned char chipid,unsigned char mask,unsigned char addr,unsigned char inst,unsigned int *data) {
  int i, ierr;
  unsigned int w;
  unsigned char len = 4;
  WaitClear(STRIP_SC_CSR,0x80000000);
  for ( i=0; i<4; i++ ) Write(STRIP_SCI+4*i,data[i]);
  w = 0x80000000 | (len<<24) | (mask<<16) | (inst<<10) | (addr<<5) | chipid;
  ierr = Write(STRIP_SC_CSR,w);
  WaitClear(STRIP_SC_CSR,0x80000000);
  return ierr;
}

void Stib::Print(unsigned char *p) {
  while ( *p != 0x00 ) {
    unsigned int seq = *p++;
    unsigned int inst = *p++;
    int ack = ( (inst&0x80) != 0 );
    inst &= 0x7f;
    unsigned int len = *p++;
    len <<= 8;
    len |= *p++;
    std::cout << "  " << dec << seq << ".  (" << len << ") " << _opname[inst];
    if ( ack ) {
      std::cout << " ack  ";
      unsigned int count = 0;
      unsigned int addr = 0;
      unsigned int data = 0;
      switch ( inst ) {
        case WAITCLR :
        case WAITSET :
          if ( len == 12  ) {
            for ( int i=0; i<4; i++ ) {
              addr <<= 8;
              addr |= *p++;
            }
            std::cout << "  0x" << hex << addr;
            if ( inst == WAITCLR ) {
              std::cout << " & ";
            }
            else {
              std::cout << " & ~";
            }
            for ( int i=0; i<4; i++ ) {
              data <<= 8;
              data |= *p++;
            }
            std::cout << "  0x" << hex << data;
            for ( int i=0; i<4; i++ ) {
              count <<= 8;
              count |= *p++;
            }
            std::cout << "  loop=" << count << std::endl;
          }
          else if ( len == 4 ) {
            for ( int i=0; i<4; i++ ) {
              count <<= 8;
              count |= *p++;
            }
            std::cout << "  loop=" << count << std::endl;
          }
          else {
            for ( unsigned int i=0; i<len; i++ ) p++;
            std::cout << std::endl;
          }
          break;
        case READ :
          if ( len == 8 ) {
            for ( int i=0; i<4; i++ ) {
              addr <<= 8;
              addr |= *p++;
            }
            std::cout << "0x" << hex << addr << " = ";
            for ( int i=0; i<4; i++ ) {
              data <<= 8;
              data |= *p++;
            }
            std::cout << "0x" << hex << data << std::endl;
          }
          else {
            for ( unsigned int i=0; i<len; i++ ) p++;
          }
          break;
        default :
          std::cout << std::endl;
          for ( unsigned int i=0; i<len; i++ ) p++;
          break;
      }
    }
    else {
      std::cout << "  ";
      unsigned int addr = 0;
      unsigned int data = 0;
      unsigned int count = 0;

      switch ( inst ) {
        case WRITE :
          for ( int i=0; i<4; i++ ) {
            addr <<= 8;
            addr |= *p++;
          }
          std::cout << "0x" << hex << addr << " <-- ";
          for ( int i=0; i<4; i++ ) {
            data <<= 8;
            data |= *p++;
          }
          std::cout << "0x" << hex << data << std::endl;
          break;
        case READ :
          for ( int i=0; i<4; i++ ) {
            addr <<= 8;
            addr |= *p++;
          }
          std::cout << "0x" << hex << addr;
          if ( len > 4 ) {
            for ( int i=0; i<4; i++ ) {
              data <<= 8;
              data |= *p++;
            }
            std::cout << " = 0x" << hex << data;
          }
          std::cout << std::endl;
          break;
        case WAITCLR :
        case WAITSET :
          for ( int i=0; i<4; i++ ) {
            addr <<= 8;
            addr |= *p++;
          }
          std::cout << "0x" << hex << addr << " <-- ";
          for ( int i=0; i<4; i++ ) {
            data <<= 8;
            data |= *p++;
          }
          std::cout << "0x" << hex << data;
          if ( len == 12 ) {
            for ( int i=0; i<4; i++ ) {
              count <<= 8;
              count |= *p++;
            }
            std::cout << " timeout=" << count;
          }
          std::cout << std::endl;
          break;
        default :
          for (unsigned int i=0; i<len; i++ ) p++;
          break;
      }
    }
  }
}

int Stib::Send() {
//  cout << "Stib::Send()..." << endl;
  int len = _pbuf-_txbuf+1;
  if ( sendto(_socket,_txbuf,len,0,_daddr,_dlen) != len ) {
    fprintf( stderr, "send() - %s\n", strerror(errno) );
    exit(1);
  }
  //usleep(100000);
  return 0;
}

int Stib::Receive() {
  int len;
  socklen_t slen = sizeof(struct sockaddr_in);
  if ( ( len = recvfrom(_socket,_rxbuf,BUFLEN,0,_daddr,&slen) ) < 0 ) {
    fprintf( stderr, "receive() - %s\n", strerror(errno) );
  }
  return len;
}

unsigned int Stib::Data(unsigned char *buf,unsigned char i,int j) {
  unsigned char *p = buf;
  unsigned int data;
  do {
    if ( *p != 0x00 ) {
      unsigned char seq = *p++;
      //unsigned char inst = *p++;
      unsigned short len = 0;
      len = *p++;
      len <<= 8;
      len |= *p++;
      if ( seq == i ) {
        if ( len/4 < j ) return -1;
        p += 4*j;
        data = 0;
        for ( int k=0; k<4; k++ ) {
          data <<= 8;
          data |= *p++;
        }
        return data;
      }
      else {
        p += len;
      }
    }
  }
  while ( *p != 0x00 );
  return -1;
}
