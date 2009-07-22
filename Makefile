
# From Hadoop's libhdfs Makefile
OS_ARCH=amd64
PLATFORM=linux
JAVA_HOME=/usr/lib/jvm/java-6-sun
N_BITS=64

JAVA_INCLUDES = -m$(N_BITS) -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(PLATFORM)
JAVA_LDFLAGS = -L$(JAVA_HOME)/jre/lib/$(OS_ARCH)/server  -ljvm -m$(N_BITS)

LIBHDFS_HOME=/home/speed/hadoop/hadoop-64bits/libhdfs-0.18.3/
LIBHDFS_INCLUDES = -I${LIBHDFS_HOME}/include
LIBHDFS_LDFLAGS = -L${LIBHDFS_HOME}/lib -lhdfs

CXXFLAGS =  -g -Wall -O0 -fPIC
CPPFLAGS =  $(LIBHDFS_INCLUDES) $(JAVA_INCLUDES)
LDFLAGS = $(LIBHDFS_LDFLAGS) $(JAVA_LDFLAGS)

TARGETS = hdfscpp.so hdfsls hdfsread hdfslistblocks hdfsdumper LDLIBPATH.txt libhdfscpp.pc

all: $(TARGETS)

hdfscpp.so: hdfscpp.cpp hdfscpp.h filebuf.h
	$(CXX) -o $@ -shared $(CPPFLAGS) $(CXXFLAGS) $< 

hdfsls: hdfsls.o hdfscpp.so
	$(CXX) $(LDFLAGS) $^ -o $@

hdfsread: hdfsread.o hdfscpp.so
	$(CXX) $(LDFLAGS) $^ -o $@

hdfslistblocks: hdfslistblocks.o hdfscpp.so
	$(CXX) $(LDFLAGS) $^ -o $@

# http://makepp.sourceforge.net/1.19/makepp_cookbook.html#Do%20you%20really%20need%20a%20library%3f
# WARNING WARNING WARNING WARNING: -lz is required for apps using this module
hdfsdumpreader_module.o: hdfsdumpreader.o hdfscpp.o
	$(LD) -r $^ -o $@ 

hdfsdumper: hdfsdumper.o hdfsdumpreader_module.o
	$(CXX) $(LDFLAGS) -lz $^ -o $@

LDLIBPATH.txt:Makefile
	echo ${LIBHDFS_HOME}/lib:$(JAVA_HOME)/jre/lib/$(OS_ARCH)/server > .$@.tmp
	mv .$@.tmp $@

libhdfscpp.pc: libhdfscpp.pc.model Makefile
	cp -f $< .$@.tmp
	sed -i.tmp.make -e "s#%PREFIX%#"`pwd`"#" \
			-e "s#%CXXFLAGS%#$(CXXFLAGS)#" \
			-e "s#%CPPFLAGS%#$(CPPFLAGS)#" \
			-e "s#%LDFLAGS%#$(LDFLAGS)#" .$@.tmp
	mv -f .$@.tmp $@
	-rm -fv .$@.tmp.tmp.make



.PHONY: clean all

clean:
	rm -fv *.o $(TARGETS) .*.tmp
