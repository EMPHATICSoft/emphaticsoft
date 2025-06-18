set(PACKAGE OnlineMonitoringViewer)
# Define the source files for the OnlineMonitoringViewer package
set(src_files
  ButtonBank.cxx
  CaptionBox.cxx
  ComparisonBox.cxx
  ComparisonOptions.cxx
  CurrentHistogram.cxx
  GUIMain.cxx
  GUIModel.cxx
  GUIModelData.cxx
  GUIModelSubscriber.cxx
  HeaderBar.cxx
  HistogramBrowser.cxx
  HistogramInfo.cxx
  HistoSource.cxx
  Icons.cxx
  LogViewer.cxx
  MenuBar.cxx
  OStream.cxx
  PlotClickHandler.cxx
  PlotOptions.cxx
  PlotViewer.cxx
  TabsBox.cxx
  UTCLabel.cxx
)
# Define the headers to include in the dictionary
set(dict_headers
  ButtonBank.h
  CaptionBox.h
  ComparisonBox.h
  ComparisonOptions.h
  CurrentHistogram.h
  GUIMain.h
  GUIModel.h
  GUIModelData.h
  GUIModelSubscriber.h
  HeaderBar.h
  HistogramBrowser.h
  HistogramInfo.h
  HistoSource.h
  Icons.h
  Layout.h
  LogViewer.h
  MenuBar.h
  OStream.h
  PlotClickerHandler.h
  PlotClickHandler.h
  PlotOptions.h
  PlotViewer.h
  TabsBox.h
  UTCLabel.h
)

# Generate the dictionary using ROOT's function directly
#ROOT_GENERATE_DICTIONARY(${PACKAGE}Dict
#  ${dict_headers}
#  LINKDEF LinkDef.h
#  MODULE ${PACKAGE}
#

cet_make_library(SOURCE ${src_files}
  LIBRARY_NAME OnlineMonitoringViewer
  LIBRARIES 
    OnlineMonitoringUtil
    ROOT::Gui
    ROOT::Gpad
    ROOT::Graf
    ROOT::Hist
  BASENAME_ONLY
)

build_dictionary(${PACKAGE}Dict
  SOURCE ${dict_headers} LinkDef.h
  DICTIONARY_LIBRARIES
    OnlineMonitoringUtil
    ROOT::Gui
    ROOT::Gpad
    ROOT::Graf
    ROOT::Hist
  LIB_TARGET OnlineMonitoringViewer
)

cet_make_exec(NAME onmon_viewer
  SOURCE onmon_viewer.cc
  LIBRARIES
    OnlineMonitoringUtil
    OnlineMonitoringViewer
)

install_source()
install_headers()
