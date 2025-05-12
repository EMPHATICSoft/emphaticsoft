// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME DataQuality_dict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "/emphaticsoft/DataQuality/classes.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR_Dictionary();
   static void artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR_TClassManip(TClass*);
   static void *new_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(void *p = nullptr);
   static void *newArray_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(Long_t size, void *p);
   static void delete_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(void *p);
   static void deleteArray_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(void *p);
   static void destruct_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::art::Wrapper<emph::dq::SpillQuality>*)
   {
      ::art::Wrapper<emph::dq::SpillQuality> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::art::Wrapper<emph::dq::SpillQuality>));
      static ::ROOT::TGenericClassInfo 
         instance("art::Wrapper<emph::dq::SpillQuality>", ::art::Wrapper<emph::dq::SpillQuality>::Class_Version(), "canvas/Persistency/Common/Wrapper.h", 79,
                  typeid(::art::Wrapper<emph::dq::SpillQuality>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR_Dictionary, isa_proxy, 4,
                  sizeof(::art::Wrapper<emph::dq::SpillQuality>) );
      instance.SetNew(&new_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR);
      instance.SetNewArray(&newArray_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR);
      instance.SetDelete(&delete_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR);
      instance.SetDeleteArray(&deleteArray_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR);
      instance.SetDestructor(&destruct_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::art::Wrapper<emph::dq::SpillQuality>*)
   {
      return GenerateInitInstanceLocal((::art::Wrapper<emph::dq::SpillQuality>*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::art::Wrapper<emph::dq::SpillQuality>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::art::Wrapper<emph::dq::SpillQuality>*)nullptr)->GetClass();
      artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR_TClassManip(theClass);
   return theClass;
   }

   static void artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR_Dictionary();
   static void artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR_TClassManip(TClass*);
   static void *new_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(void *p = nullptr);
   static void *newArray_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(Long_t size, void *p);
   static void delete_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(void *p);
   static void deleteArray_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(void *p);
   static void destruct_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::art::Wrapper<emph::dq::EventQuality>*)
   {
      ::art::Wrapper<emph::dq::EventQuality> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::art::Wrapper<emph::dq::EventQuality>));
      static ::ROOT::TGenericClassInfo 
         instance("art::Wrapper<emph::dq::EventQuality>", ::art::Wrapper<emph::dq::EventQuality>::Class_Version(), "canvas/Persistency/Common/Wrapper.h", 79,
                  typeid(::art::Wrapper<emph::dq::EventQuality>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR_Dictionary, isa_proxy, 4,
                  sizeof(::art::Wrapper<emph::dq::EventQuality>) );
      instance.SetNew(&new_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR);
      instance.SetNewArray(&newArray_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR);
      instance.SetDelete(&delete_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR);
      instance.SetDeleteArray(&deleteArray_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR);
      instance.SetDestructor(&destruct_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::art::Wrapper<emph::dq::EventQuality>*)
   {
      return GenerateInitInstanceLocal((::art::Wrapper<emph::dq::EventQuality>*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::art::Wrapper<emph::dq::EventQuality>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::art::Wrapper<emph::dq::EventQuality>*)nullptr)->GetClass();
      artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR_TClassManip(theClass);
   return theClass;
   }

   static void artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *emphcLcLdqcLcLEventQuality_Dictionary();
   static void emphcLcLdqcLcLEventQuality_TClassManip(TClass*);
   static void *new_emphcLcLdqcLcLEventQuality(void *p = nullptr);
   static void *newArray_emphcLcLdqcLcLEventQuality(Long_t size, void *p);
   static void delete_emphcLcLdqcLcLEventQuality(void *p);
   static void deleteArray_emphcLcLdqcLcLEventQuality(void *p);
   static void destruct_emphcLcLdqcLcLEventQuality(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::emph::dq::EventQuality*)
   {
      ::emph::dq::EventQuality *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::emph::dq::EventQuality));
      static ::ROOT::TGenericClassInfo 
         instance("emph::dq::EventQuality", 13, "DataQuality/EventQuality.h", 12,
                  typeid(::emph::dq::EventQuality), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &emphcLcLdqcLcLEventQuality_Dictionary, isa_proxy, 12,
                  sizeof(::emph::dq::EventQuality) );
      instance.SetNew(&new_emphcLcLdqcLcLEventQuality);
      instance.SetNewArray(&newArray_emphcLcLdqcLcLEventQuality);
      instance.SetDelete(&delete_emphcLcLdqcLcLEventQuality);
      instance.SetDeleteArray(&deleteArray_emphcLcLdqcLcLEventQuality);
      instance.SetDestructor(&destruct_emphcLcLdqcLcLEventQuality);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::emph::dq::EventQuality*)
   {
      return GenerateInitInstanceLocal((::emph::dq::EventQuality*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::emph::dq::EventQuality*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *emphcLcLdqcLcLEventQuality_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::emph::dq::EventQuality*)nullptr)->GetClass();
      emphcLcLdqcLcLEventQuality_TClassManip(theClass);
   return theClass;
   }

   static void emphcLcLdqcLcLEventQuality_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *emphcLcLdqcLcLSpillQuality_Dictionary();
   static void emphcLcLdqcLcLSpillQuality_TClassManip(TClass*);
   static void *new_emphcLcLdqcLcLSpillQuality(void *p = nullptr);
   static void *newArray_emphcLcLdqcLcLSpillQuality(Long_t size, void *p);
   static void delete_emphcLcLdqcLcLSpillQuality(void *p);
   static void deleteArray_emphcLcLdqcLcLSpillQuality(void *p);
   static void destruct_emphcLcLdqcLcLSpillQuality(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::emph::dq::SpillQuality*)
   {
      ::emph::dq::SpillQuality *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::emph::dq::SpillQuality));
      static ::ROOT::TGenericClassInfo 
         instance("emph::dq::SpillQuality", 12, "DataQuality/SpillQuality.h", 13,
                  typeid(::emph::dq::SpillQuality), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &emphcLcLdqcLcLSpillQuality_Dictionary, isa_proxy, 12,
                  sizeof(::emph::dq::SpillQuality) );
      instance.SetNew(&new_emphcLcLdqcLcLSpillQuality);
      instance.SetNewArray(&newArray_emphcLcLdqcLcLSpillQuality);
      instance.SetDelete(&delete_emphcLcLdqcLcLSpillQuality);
      instance.SetDeleteArray(&deleteArray_emphcLcLdqcLcLSpillQuality);
      instance.SetDestructor(&destruct_emphcLcLdqcLcLSpillQuality);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::emph::dq::SpillQuality*)
   {
      return GenerateInitInstanceLocal((::emph::dq::SpillQuality*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::emph::dq::SpillQuality*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *emphcLcLdqcLcLSpillQuality_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::emph::dq::SpillQuality*)nullptr)->GetClass();
      emphcLcLdqcLcLSpillQuality_TClassManip(theClass);
   return theClass;
   }

   static void emphcLcLdqcLcLSpillQuality_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::art::Wrapper<emph::dq::SpillQuality> : new ::art::Wrapper<emph::dq::SpillQuality>;
   }
   static void *newArray_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::art::Wrapper<emph::dq::SpillQuality>[nElements] : new ::art::Wrapper<emph::dq::SpillQuality>[nElements];
   }
   // Wrapper around operator delete
   static void delete_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(void *p) {
      delete ((::art::Wrapper<emph::dq::SpillQuality>*)p);
   }
   static void deleteArray_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(void *p) {
      delete [] ((::art::Wrapper<emph::dq::SpillQuality>*)p);
   }
   static void destruct_artcLcLWrapperlEemphcLcLdqcLcLSpillQualitygR(void *p) {
      typedef ::art::Wrapper<emph::dq::SpillQuality> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::art::Wrapper<emph::dq::SpillQuality>

namespace ROOT {
   // Wrappers around operator new
   static void *new_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::art::Wrapper<emph::dq::EventQuality> : new ::art::Wrapper<emph::dq::EventQuality>;
   }
   static void *newArray_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::art::Wrapper<emph::dq::EventQuality>[nElements] : new ::art::Wrapper<emph::dq::EventQuality>[nElements];
   }
   // Wrapper around operator delete
   static void delete_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(void *p) {
      delete ((::art::Wrapper<emph::dq::EventQuality>*)p);
   }
   static void deleteArray_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(void *p) {
      delete [] ((::art::Wrapper<emph::dq::EventQuality>*)p);
   }
   static void destruct_artcLcLWrapperlEemphcLcLdqcLcLEventQualitygR(void *p) {
      typedef ::art::Wrapper<emph::dq::EventQuality> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::art::Wrapper<emph::dq::EventQuality>

namespace ROOT {
   // Wrappers around operator new
   static void *new_emphcLcLdqcLcLEventQuality(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::emph::dq::EventQuality : new ::emph::dq::EventQuality;
   }
   static void *newArray_emphcLcLdqcLcLEventQuality(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::emph::dq::EventQuality[nElements] : new ::emph::dq::EventQuality[nElements];
   }
   // Wrapper around operator delete
   static void delete_emphcLcLdqcLcLEventQuality(void *p) {
      delete ((::emph::dq::EventQuality*)p);
   }
   static void deleteArray_emphcLcLdqcLcLEventQuality(void *p) {
      delete [] ((::emph::dq::EventQuality*)p);
   }
   static void destruct_emphcLcLdqcLcLEventQuality(void *p) {
      typedef ::emph::dq::EventQuality current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::emph::dq::EventQuality

namespace ROOT {
   // Wrappers around operator new
   static void *new_emphcLcLdqcLcLSpillQuality(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::emph::dq::SpillQuality : new ::emph::dq::SpillQuality;
   }
   static void *newArray_emphcLcLdqcLcLSpillQuality(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::emph::dq::SpillQuality[nElements] : new ::emph::dq::SpillQuality[nElements];
   }
   // Wrapper around operator delete
   static void delete_emphcLcLdqcLcLSpillQuality(void *p) {
      delete ((::emph::dq::SpillQuality*)p);
   }
   static void deleteArray_emphcLcLdqcLcLSpillQuality(void *p) {
      delete [] ((::emph::dq::SpillQuality*)p);
   }
   static void destruct_emphcLcLdqcLcLSpillQuality(void *p) {
      typedef ::emph::dq::SpillQuality current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::emph::dq::SpillQuality

namespace {
  void TriggerDictionaryInitialization_libDataQuality_dict_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/emphaticsoft",
"/build",
"/emphaticsoft",
"/cvmfs/emphatic.opensciencegrid.org/products/canvas/v3_14_00/include",
"/cvmfs/emphatic.opensciencegrid.org/products/fhiclcpp/v4_17_00/include",
"/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/include",
"/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/include",
"/cvmfs/emphatic.opensciencegrid.org/products/boost/v1_80_0/Linux64bit+3.10-2.17-e20-prof/include",
"/cvmfs/emphatic.opensciencegrid.org/products/boost/v1_80_0/Linux64bit+3.10-2.17-e20-prof/include",
"/cvmfs/emphatic.opensciencegrid.org/products/boost/v1_80_0/Linux64bit+3.10-2.17-e20-prof/include",
"/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/include",
"/cvmfs/emphatic.opensciencegrid.org/products/cetlib_except/v1_08_00/include",
"/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/include",
"/usr/include",
"/cvmfs/emphatic.opensciencegrid.org/products/hep_concurrency/v1_08_00/slf7.x86_64.e20.prof/include",
"/cvmfs/emphatic.opensciencegrid.org/products/tbb/v2021_7_0/Linux64bit+3.10-2.17-e20/include",
"/build",
"/emphaticsoft",
"/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/include",
"/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/include/",
"/build/DataQuality/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libDataQuality_dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
namespace emph{namespace dq{class __attribute__((annotate("$clingAutoload$DataQuality/SpillQuality.h")))  SpillQuality;}}
namespace art{template <typename T> class __attribute__((annotate("$clingAutoload$canvas/Persistency/Common/Wrapper.h")))  Wrapper;
}
namespace emph{namespace dq{class __attribute__((annotate("$clingAutoload$DataQuality/EventQuality.h")))  EventQuality;}}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libDataQuality_dict dictionary payload"

#ifndef NDEBUG
  #define NDEBUG 1
#endif
#ifndef _REENTRANT
  #define _REENTRANT 1
#endif
#ifndef BOOST_FILESYSTEM_NO_LIB
  #define BOOST_FILESYSTEM_NO_LIB 1
#endif
#ifndef BOOST_FILESYSTEM_DYN_LINK
  #define BOOST_FILESYSTEM_DYN_LINK 1
#endif
#ifndef BOOST_ATOMIC_NO_LIB
  #define BOOST_ATOMIC_NO_LIB 1
#endif
#ifndef BOOST_ATOMIC_DYN_LINK
  #define BOOST_ATOMIC_DYN_LINK 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "canvas/Persistency/Common/Wrapper.h"

#include "DataQuality/EventQuality.h"
#include "DataQuality/SpillQuality.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"art::Wrapper<emph::dq::EventQuality>", payloadCode, "@",
"art::Wrapper<emph::dq::SpillQuality>", payloadCode, "@",
"emph::dq::EventQuality", payloadCode, "@",
"emph::dq::SpillQuality", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libDataQuality_dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libDataQuality_dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libDataQuality_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libDataQuality_dict() {
  TriggerDictionaryInitialization_libDataQuality_dict_Impl();
}
