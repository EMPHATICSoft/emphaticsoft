#ifndef _PxSuite_PxEvent_h
#define _PxSuite_PxEvent_h

#include <stdint.h>
#include <vector>
#include <map>
#include <Hit.h>

//namespace PxSuite
//{


class PxEvent //: public TObject
{
public:
    PxEvent(void);
    PxEvent(int64_t bcoNumber);
    virtual ~PxEvent(void);

    //Getters
    int32_t                          getTriggerNumber   (void) const;
    int64_t                          getTriggerEventBCO (void) const;
    int64_t                          getTriggerBCO      (void) const;
    int64_t                          getBCONumber       (void) const;
    int64_t                          getCurrentBCO      (void) const;
    unsigned int                     getNumberOfHits    (void) const;
    uint32_t                         getRawHit          (unsigned int position) const;
    const std::vector<uint32_t>&     getRawHits         (void) const;
    Hit                              getDecodedHit      (unsigned int position) const;
    const std::vector<Hit>&          getDecodedHits     (void) const;

    //Setters
    void setTriggerNumber   (int32_t  triggerNumber);
    void setTriggerEventBCO (int64_t  triggerEventBCO);
    void setTriggerBCO      (int64_t  triggerBCO);
    void setBCONumber       (int64_t  bcoNumber);
    void setCurrentBCO      (int64_t  currentbco);
    void setRawHit          (uint32_t rawHit);
    void setRawHits         (const std::vector<uint32_t>& rawHits);
    void setDecodedHit      (const Hit& Hit);
    void setDecodedHits     (const std::vector<Hit>& Hits);
    

    void         addTriggerNumber    (int station, int32_t  triggerNumber);
    unsigned int getNumberOfTriggers (void);
    void         addTriggerNumberToHits(void);

    void         printTriggerInfo (void);
    //Operators
    PxEvent operator+=(const PxEvent& event);


private:
    int32_t triggerNumber_;   //-1 means untriggered event
    int64_t triggerEventBCO_;
    int64_t triggerBCO_;
    int64_t bcoNumber_;       //-1 means no bco information
    int64_t currentbco_;

    std::vector<uint32_t>  rawHits_;
    std::vector<Hit>       decodedHits_;
    std::map<int, int32_t> triggerNumbers_;

    //ClassDef(PxEvent,1);
};

//}

#endif
