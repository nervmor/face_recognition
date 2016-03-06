

all:build



build:
	cd voice_detect_lib;$(MAKE)
	cd voice_detector; $(MAKE)
	-mkdir ./target
	cp -avxrf ./voice_detector/voice_detector ./target/voice_detector
	cp -avxrf ./config/voice_detector.cfg ./target/voice_detector.cfg
	cp -avxrf ./third/Talentedsoft_tsVPR3.0_SDK_Linux64_20150107/lib/tsVPR.ini ./target/tsVPR.ini
	-mkdir ./target/engine
	cp -avxrf ./third/Talentedsoft_tsVPR3.0_SDK_Linux64_20150107/engine/* ./target/engine
	-mkdir ./target/test_data
	cp -avxrf ./test_data/* ./target/test_data
	
clean:
	cd voice_detect_lib;$(MAKE) clean
	cd voice_detector; $(MAKE) clean
	-rm -fr ./target
	
	
prebuild:
	cd third/boost_1_58_0; ./bootstrap.sh; ./b2 cxxflags='-fPIC' stage link=static runtime-link=shared threading=multi release --with-date_time --with-thread --with-filesystem --with-locale
	cp -avxrf ./third/Talentedsoft_tsVPR3.0_SDK_Linux64_20150107/lib/libtsVPR.so /usr/lib/libtsVPR.so