#ifndef __STIB__
#define __STIB__

namespace stib {

  const unsigned int STRIP_CSR = 0xc4000000;
  const unsigned int STRIP_RESET = 0xc4000004;
  const unsigned int STRIP_SC_CSR = 0xc4000008;
  const unsigned int STRIP_SCI = 0xc4000010;
  const unsigned int STRIP_SCI0 = 0xc4000010;
  const unsigned int STRIP_SCI1 = 0xc4000014;
  const unsigned int STRIP_SCI2 = 0xc4000018;
  const unsigned int STRIP_SCI3 = 0xc400001c;
  const unsigned int STRIP_SCO = 0xc4000020;
  const unsigned int STRIP_SCO0 = 0xc4000020;
  const unsigned int STRIP_SCO1 = 0xc4000024;
  const unsigned int STRIP_SCO2 = 0xc4000028;
  const unsigned int STRIP_SCO3 = 0xc400002c;
  const unsigned int STRIP_FIFO_CSR = 0xc4000030;
  const unsigned int STRIP_FIFO_DATA = 0xc4000034;
  const unsigned int STRIP_ANALYSIS_CSR = 0xc4000038;
  const unsigned int STRIP_ANALYSIS_BCO_COUNTER = 0xc400003c;
  const unsigned int STRIP_ANALYSIS_CHIP1_HIT_COUNTER = 0xc400013c;
  const unsigned int STRIP_ANALYSIS_CHIP2_HIT_COUNTER = 0xc400023c;
  const unsigned int STRIP_ANALYSIS_CHIP3_HIT_COUNTER = 0xc400033c;
  const unsigned int STRIP_ANALYSIS_CHIP4_HIT_COUNTER = 0xc400043c;
  const unsigned int STRIP_ANALYSIS_CHIP5_HIT_COUNTER = 0xc400053c;
  const unsigned int STRIP_CHIP1_STATUS = 0xc4000130;
  const unsigned int STRIP_CHIP2_STATUS = 0xc4000230;
  const unsigned int STRIP_CHIP3_STATUS = 0xc4000330;
  const unsigned int STRIP_CHIP4_STATUS = 0xc4000430;
  const unsigned int STRIP_CHIP5_STATUS = 0xc4000530;
  const unsigned int STRIP_BCO_COUNTER = 0xc4000040;
  const unsigned int STRIP_BCO_DCM = 0xc4000048;
  const unsigned int STRIP_DAC_CSR = 0xc4000050;
  const unsigned int STRIP_DAC_INPUT = 0xc4000054;
  const unsigned int STRIP_DAC_SPI = 0xc4000058;
  const unsigned int STRIP_TRIG_CSR = 0xc4000060;
  const unsigned int STRIP_TRIG_UNBIASED = 0xc4000068;
  const unsigned int STRIP_TRIG_INPUT_0 = 0xc4000070;
  const unsigned int STRIP_TRIG_INPUT_1 = 0xc4000074;
  const unsigned int STRIP_TRIG_INPUT_2 = 0xc4000078;
  const unsigned int STRIP_TRIG_INPUT_3 = 0xc400007c;
  const unsigned int DATA_DESTINATION_IP   = 0xc10000a4;
  const unsigned int DATA_SOURCE_DESTINATION_PORT = 0xc10000a8;

  const unsigned int STRIP_TRIM_CSR = 0xc4000088;
  const unsigned int STRIP_TLK_CSR = 0xc3000000;

  const unsigned short STIB_PORT = 47000;

  const unsigned char WRITE = 1;
  const unsigned char SET = 2;
  const unsigned char READ = 4;
  const unsigned char RESET = 5;
  const unsigned char CLEAR = 5;
  const unsigned char DEFAULT = 6;
  const unsigned char WAITCLR = 7;
  const unsigned char WAITSET = 8;

  const unsigned char PULSER_DATA = 1;
  const unsigned char PULSER_CONTROL = 2;
  const unsigned char VBN = 3;
  const unsigned char SHPVBP2 = 4;
  const unsigned char SHPVBP1 = 5;
  const unsigned char BLRVBP1 = 6;
  const unsigned char DVTN = 7;
  const unsigned char DTHR = 8;
  const unsigned char DTHR0 = 8;
  const unsigned char DTHR1 = 9;
  const unsigned char DTHR2 = 10;
  const unsigned char DTHR3 = 11;
  const unsigned char DTHR4 = 12;
  const unsigned char DTHR5 = 13;
  const unsigned char DTHR6 = 14;
  const unsigned char DTHR7 = 15;
  const unsigned char ALINES = 16;
  const unsigned char KILL = 17;
  const unsigned char INJECT = 18;
  const unsigned char SEND_DATA = 19;
  const unsigned char REJECT_HITS = 20;
  const unsigned char SPR = 24;
  const unsigned char DCR = 27;
  const unsigned char SCR = 28;
  const unsigned char ACQUIRE_BCO = 30;

  const int BUFLEN = 1024;

  class Stib {
    int _socket;
    struct sockaddr *_daddr;
    int _dlen;
    unsigned int _id;
    unsigned char *_pbuf;
    unsigned int _iseq;
    unsigned char _txbuf[BUFLEN];
    unsigned char _rxbuf[BUFLEN];
    static const char *_opname[];
  public:
    static void MaskBits(unsigned int *,unsigned char);
    static void MaskBits(unsigned int *,unsigned char,int);
    static unsigned char StripNumber(unsigned char);
    static unsigned char SetNumber(unsigned char);
    static unsigned int SensorStrip(int,int,int);
    Stib(int,struct sockaddr *,int);
    void Clear();
    int Write(unsigned int,unsigned int);
    int SlowControls(unsigned char,unsigned char,unsigned char,unsigned char);
    int SlowControls(unsigned char,unsigned char,unsigned char,unsigned char,unsigned char);
    int SlowControls(unsigned char,unsigned char,unsigned char,unsigned char,unsigned int *);
    int WaitClear(unsigned int,unsigned int,unsigned int=256);
    int WaitSet(unsigned int,unsigned int,unsigned int=256);
    int Read(unsigned int);
    void Print(unsigned char *);
    void PrintTx() { Print(_txbuf); }
    void PrintRx() { Print(_rxbuf); }
    int Send();
    int Receive();
    unsigned int Data(unsigned char *,unsigned char i,int j);
    unsigned int RxData(unsigned char i,int j) { return Data(_rxbuf,i,j); }
  };
}
#endif
