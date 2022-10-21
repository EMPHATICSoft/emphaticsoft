include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

build_plugin(MakeSSDStationClusters "module"
	LIBRARIES PRIVATE
	RawData
	RecoBase
)

build_plugin(MakeSSDClusters "module"
	LIBRARIES PRIVATE
	Geometry
	RawData
	RecoBase
)

install_fhicl()
