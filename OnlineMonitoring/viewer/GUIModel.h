#ifndef OM_GUIMODEL_H
#define OM_GUIMODEL_H
#include <vector>
#include <string>
#include "OnlineMonitoring/viewer/GUIModelData.h"

namespace om { class GUIModelSubscriber; }

namespace om
{
  class GUIModel {
  public:
    static GUIModel& Instance();

    const GUIModelData& Data() const { return fData; }

    void Subscribe(GUIModelSubscriber* s, unsigned int which);
    void Unsubscribe(GUIModelSubscriber* s);

    void HistogramBack();
    void HistogramForward();
    void SetDetector(const char* n);
    void SetHistogramSource(const char* s);
    void SetCurrentHistogram(const char* h);
    void SetCSVFile(std::string v);
    void SetHistogramInfoText(const char* t);
    void SetPlotViewRefreshPaused(bool paused);
    void DrawContentProjection();
    void SendUpdates() const;

    void SetCompareWhich(unsigned int i);
    void SetReferenceFile(const char* RefFile);
    void SetLookBack(int lb);
    void SetCompareMethod(unsigned int i);
    void SetCompareNorm(unsigned int i);

    void SendPrint();

  public:
    void Publish(unsigned int which) const;
    void Init();

  private:
    GUIModel();
  private:
    GUIModelData                     fData;
    std::vector<GUIModelSubscriber*> fSubscriber;
    std::vector<unsigned int>        fWhich;
  };
}

#endif
////////////////////////////////////////////////////////////////////////
