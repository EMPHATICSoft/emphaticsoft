#ifndef OM_GUIMODELSUBSCRIBER_H
#define OM_GUIMODELSUBSCRIBER_H
namespace emph { namespace onmon { class GUIModelData; } }

namespace emph { 
  namespace onmon {
    ///
    /// Any class which needs to be informated about state changes in
    /// the GUIModel
    ///
    class GUIModelSubscriber
    {
    public:
      ///
      /// Create a GUIModel subscriber.
      ///
      /// \param which - bit set indicating which messages we want
      ///
      /// See GUIModelData for the list of possibilities
      ///
      GUIModelSubscriber(unsigned int which);
      virtual ~GUIModelSubscriber();

      bool GetLock();
      void ReleaseLock();

      ///
      /// Receive notifications when the GUI model data has changed
      ///
      /// \param m     - The new model data
      /// \param which - Which element of the data has changed
      ///
      /// See GUIModel.h for the definitions of "which".
      ///
      /// Note: To prevent collisions between threads, users should
      /// implement their "GUIModelDataIssue" methods following this
      /// pattern:
      ///
      /// {
      ///   book aok = this->GetLock();
      ///   if (!aok) return;
      ///
      ///   ...your code here...
      ///
      ///   this->ReleaseLock();
      /// }
      ///
      virtual void GUIModelDataIssue(const GUIModelData& m,
  				   unsigned int which) = 0;
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
