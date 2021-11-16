//
//  emphatic-artdaq/Generators/CAENV1730Readout.hh
//

#ifndef emphatic_artdaq_Generators_CAENV1730Readout_hh
#define emphatic_artdaq_Generators_CAENV1730Readout_hh

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/fwd.h"
#include "artdaq-core/Data/Fragment.hh"
#include "artdaq/Generators/CommandableFragmentGenerator.hh"

#include "CAENDigitizer.h"
#include "CAENDigitizerType.h"
#include "emphatic-artdaq/Overlays/CAENV1730Fragment.hh"

#include "CAENConfiguration.hh"

//#include "CircularBuffer.hh"
#include "PoolBuffer.hh"
#include "workerThread.hh"

#include <string>
#include <unordered_map>
#include <mutex>

namespace emphaticdaq
{

  class CAENV1730Readout : public artdaq::CommandableFragmentGenerator{

  public:

    explicit CAENV1730Readout(fhicl::ParameterSet const& ps);
    virtual ~CAENV1730Readout();

    bool getNext_(artdaq::FragmentPtrs & output) override;
    bool checkHWStatus_() override;
    void start() override;
    void stop() override;
    void stopNoMutex() override { stop(); }
    //void init();

  private:
    bool readSingleWindowFragments(artdaq::FragmentPtrs &);
    bool readSingleWindowDataBlock();
    bool readWindowDataBlocks();

    bool readCombinedWindowFragments(artdaq::FragmentPtrs &);

    void loadConfiguration(fhicl::ParameterSet const& ps);
    void configureInterrupts();

    typedef enum
    {
      CONFIG_READ_ADDR     = 0x8000,
      CONFIG_SET_ADDR      = 0x8004,
      CONFIG_CLEAR_ADDR    = 0x8008,
      TRIGGER_OVERLAP_MASK = 0x0002
    } REGISTERS_t;

    //CAEN pieces
    CAENConfiguration     fCAEN;	// initialized in the constructor
    int                   fHandle;
    CAEN_DGTZ_BoardInfo_t fBoardInfo;
    //char*                 fBuffer;
    uint32_t              fBufferSize;
    //uint32_t              fCircularBufferSize;
    CAEN_DGTZ_AcqMode_t   fAcqMode;	// initialized in the constructor


    typedef enum {
      TEST_PATTERN_S=3
    } TEST_PATTERN_t;

    typedef enum {
      BOARD_READY  = 0x0100,
      PLL_STATUS   = 0x0080,
      PLL_BYPASS   = 0x0040,
      CLOCK_SOURCE = 0x0020,
      EVENT_FULL   = 0x0010,
      EVENT_READY  = 0x0008,
      RUN_ENABLED  = 0x0004
    } ACQ_STATUS_MASK_t;

    typedef enum {
      DYNAMIC_RANGE      = 0x8028,
      TRG_OUT_WIDTH      = 0x8070,
      TRG_OUT_WIDTH_CH   = 0x1070,
      ACQ_CONTROL        = 0x8100,
      FP_TRG_OUT_CONTROL = 0x8110,
      FP_IO_CONTROL      = 0x811C,
      FP_LVDS_CONTROL    = 0x81A0,
      READOUT_CONTROL    = 0xEF00,
      // Animesh & Aiwu add registers for the LVDS logic
      FP_LVDS_Logic_G1   = 0x1084,
      FP_LVDS_Logic_G2   = 0x1184,
      FP_LVDS_Logic_G3   = 0x1284,
      FP_LVDS_Logic_G4   = 0x1384,
      FP_LVDS_Logic_G5   = 0x1484,
      FP_LVDS_Logic_G6   = 0x1584,
      FP_LVDS_Logic_G7   = 0x1684,
      FP_LVDS_Logic_G8   = 0x1784,
      // Animesh & Aiwu add end
      // Animesh & Aiwu add registers for the LVDS output width
      FP_LVDS_OutWidth_Ch1   = 0x1070,
      FP_LVDS_OutWidth_Ch2   = 0x1170,
      FP_LVDS_OutWidth_Ch3   = 0x1270,
      FP_LVDS_OutWidth_Ch4   = 0x1370,
      FP_LVDS_OutWidth_Ch5   = 0x1470,
      FP_LVDS_OutWidth_Ch6   = 0x1570,
      FP_LVDS_OutWidth_Ch7   = 0x1670,
      FP_LVDS_OutWidth_Ch8   = 0x1770,
      FP_LVDS_OutWidth_Ch9   = 0x1870,
      FP_LVDS_OutWidth_Ch10   = 0x1970,
      FP_LVDS_OutWidth_Ch11   = 0x1A70,
      FP_LVDS_OutWidth_Ch12   = 0x1B70,
      FP_LVDS_OutWidth_Ch13   = 0x1C70,
      FP_LVDS_OutWidth_Ch14   = 0x1D70,
      FP_LVDS_OutWidth_Ch15   = 0x1E70,
      FP_LVDS_OutWidth_Ch16   = 0x1F70,
      // Animesh & Aiwu add - check DPP algorithm feature
      DPP_Alo_Feature_Ch1 = 0x1080,
      DPP_Alo_Feature_Ch2 = 0x1180,
      Baseline_Ch1 = 0x1098,
      Baseline_Ch2 = 0x1198,
      Baseline_Ch3 = 0x1298,
      Baseline_Ch4 = 0x1398,
      Baseline_Ch5 = 0x1498,
      Baseline_Ch6 = 0x1598,
      Baseline_Ch7 = 0x1698,
      Baseline_Ch8 = 0x1798,
      Baseline_Ch9 = 0x1898,
      Baseline_Ch10 = 0x1998,
      Baseline_Ch11 = 0x1A98,
      Baseline_Ch12 = 0x1B98,
      Baseline_Ch13 = 0x1C98,
      Baseline_Ch14 = 0x1D98,
      Baseline_Ch15 = 0x1E98,
      Baseline_Ch16 = 0x1F98,
      // want to send a software trigger
      // SWTriggerValue = 0x8108
      // Animesh & Aiwu add end
    } ADDRESS_t;

    typedef enum
    {
      ENABLE_LVDS_TRIGGER  = 0x20000000,
      ENABLE_EXT_TRIGGER   = 0x40000000,
      ENABLE_NEW_LVDS      = 0x100,
      ENABLE_TRG_OUT       = 0xFF,
      TRG_IN_LEVEL         = 0x400,
      TRIGGER_LOGIC        = 0x1F00,
      DISABLE_TRG_OUT_LEMO = 0x2,
      LVDS_IO              = 0x3C,
      LVDS_BUSY            = 0,
      LVDS_TRIGGER         = 1,
      LVDS_nBUSY_nVETO     = 2,
      LVDS_LEGACY          = 3
    } IO_MASK_t;

    enum {
      TERROR    = TLVL_ERROR,
      TWARNING  = TLVL_WARNING,
      TINFO     = TLVL_INFO,
      TDEBUG    = TLVL_DEBUG,
      TCONFIG   = 9,
      TSTART    = 10,
      TSTOP     = 11,
      TSTATUS   = 12,
      TGETNEXT  = 13,
      TGETDATA  = 14,
      TMAKEFRAG = 15,
      TTEMP     = 30
    };

    //fhicl parameters
    int fVerbosity;
    int fBoardChainNumber;
    uint8_t  fInterruptEnable;
    uint32_t fIRQTimeoutMS;
    uint32_t fGetNextSleep;
    uint32_t fGetNextFragmentBunchSize;
    uint32_t fMaxEventsPerTransfer;
    bool     fSWTrigger;
    uint32_t fSelfTriggerMode;
    uint32_t fSelfTriggerMask;
    uint32_t fModeLVDS;
    uint32_t fTrigOutDelay;
    uint32_t fTrigInLevel;
    bool     fCombineReadoutWindows;
    bool     fCalibrateOnConfig;
    bool     fLockTempCalibration;
    bool     fWriteCalibration;
    uint32_t fFragmentID;

    bool fUseTimeTagForTimeStamp;
    uint32_t fTimeOffsetNanoSec;

    // Animesh & Aiwu add fhicl parameters - LVDS logic
    uint32_t fLVDSLogicValueG1;
    uint32_t fLVDSLogicValueG2;
    uint32_t fLVDSLogicValueG3;
    uint32_t fLVDSLogicValueG4;
    uint32_t fLVDSLogicValueG5;
    uint32_t fLVDSLogicValueG6;
    uint32_t fLVDSLogicValueG7;
    uint32_t fLVDSLogicValueG8;
    // Animesh & Aiwu add end
    // Animesh & Aiwu add fhicl parameters - LVDS output pulse width
    uint32_t fLVDSOutWidthC1;
    uint32_t fLVDSOutWidthC2;
    uint32_t fLVDSOutWidthC3;
    uint32_t fLVDSOutWidthC4;
    uint32_t fLVDSOutWidthC5;
    uint32_t fLVDSOutWidthC6;
    uint32_t fLVDSOutWidthC7;
    uint32_t fLVDSOutWidthC8;
    uint32_t fLVDSOutWidthC9;
    uint32_t fLVDSOutWidthC10;
    uint32_t fLVDSOutWidthC11;
    uint32_t fLVDSOutWidthC12;
    uint32_t fLVDSOutWidthC13;
    uint32_t fLVDSOutWidthC14;
    uint32_t fLVDSOutWidthC15;
    uint32_t fLVDSOutWidthC16;
    // Animesh & Aiwu add end
    //Animesh & Aiwu add - self trigger polarity
    uint32_t fSelfTrigBit;
    //Animesh & Aiwu add end
    // Animesh & Aiwu add - dpp algorithm feature 0x1n80
    uint32_t fChargePedstalBitCh1;
    uint32_t fBaselineCh1;
    uint32_t fBaselineCh2;
    uint32_t fBaselineCh3;
    uint32_t fBaselineCh4;
    uint32_t fBaselineCh5;
    uint32_t fBaselineCh6;
    uint32_t fBaselineCh7;
    uint32_t fBaselineCh8;
    uint32_t fBaselineCh9;
    uint32_t fBaselineCh10;
    uint32_t fBaselineCh11;
    uint32_t fBaselineCh12;
    uint32_t fBaselineCh13;
    uint32_t fBaselineCh14;
    uint32_t fBaselineCh15;
    uint32_t fBaselineCh16;
    //uint32_t fSWTriggerValue;
    // Animesh & Aiwu add end


    //internals
    size_t   fNChannels;
    uint32_t fBoardID;
    bool     fOK;
    bool     fail_GetNext;
    uint32_t fEvCounter; // set to zero at the beginning
    uint32_t last_rcvd_rwcounter;
    uint32_t last_sent_rwcounter;
    uint32_t last_sent_ts;
    uint32_t total_data_size;
    //uint32_t event_size;
    uint32_t n_readout_windows;
    uint32_t ch_temps[CAENConfiguration::MAX_CHANNELS];
    uint32_t ch_status[CAENConfiguration::MAX_CHANNELS];

    //functions
    void Configure();

    void ConfigureRecordFormat();
    void ConfigureDataBuffer();
    void ConfigureTrigger();
    void ConfigureReadout();
    void ConfigureAcquisition();
    void ConfigureLVDS();
    void ConfigureOthers();
    void ConfigureSelfTriggerMode();
    void RunADCCalibration();
    void SetLockTempCalibration(bool onOff, uint32_t ch);
    CAEN_DGTZ_ErrorCode WriteSPIRegister(int handle, uint32_t ch, uint32_t address, uint8_t value);
    CAEN_DGTZ_ErrorCode ReadSPIRegister(int handle, uint32_t ch, uint32_t address, uint8_t *value);
    void Read_ADC_CalParams_V1730(int handle, int ch, uint8_t *CalParams);
    void Write_ADC_CalParams_V1730(int handle, int ch, uint8_t *CalParams);
    void ReadChannelBusyStatus(int handle, uint32_t ch, uint32_t& status);


    bool WaitForTrigger();
    bool GetData();
    share::WorkerThreadUPtr GetData_thread_;
    emphaticdaq::PoolBuffer fPoolBuffer;
    size_t fCircularBufferSize;
    std::unique_ptr<uint16_t[]> fBuffer;

    std::unordered_map<uint32_t,artdaq::Fragment::timestamp_t> fTimestampMap;
    mutable std::mutex fTimestampMapMutex;

    //internals in getting the data
    boost::posix_time::ptime fTimePollEnd,fTimePollBegin;
    boost::posix_time::ptime fTimeEpoch;
    boost::posix_time::time_duration fTimeDiffPollBegin,fTimeDiffPollEnd;

    artdaq::Fragment::timestamp_t fTS;
    uint64_t fMeanPollTime;
    uint64_t fMeanPollTimeNS;
    uint32_t fTTT;
    long fTTT_ns;

    void CheckReadback(std::string,int,uint32_t,uint32_t,int channelID=-1);

    CAEN_DGTZ_ErrorCode	WriteRegisterBitmask(int32_t handle, uint32_t address,
					     uint32_t data, uint32_t bitmask);

  };

}

#endif
