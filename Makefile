
# From Hadoop's libhdfs Makefile
OS_ARCH=i386
PLATFORM=linux
JAVA_HOME=/usr/lib/jvm/java-6-sun
N_BITS=32

JAVA_INCLUDES = -m$(N_BITS) -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(PLATFORM)
JAVA_LDFLAGS = -L$(JAVA_HOME)/jre/lib/$(OS_ARCH)/server  -ljvm -shared -m$(N_BITS) -Wl,-x 

HADOOP_HOME=/home/speed/hadoop/hadoop-64bits/hadoop
HADOOP_INCLUDES = -I${HADOOP_HOME}/src/c++/libhdfs

CXXFLAGS   =  $(HADOOP_INCLUDES) $(JAVA_INCLUDES) -g -O0 -shared -fPIC
LDFLAGS = -L${HADOOP_HOME}/libhdfs
LDLIBS   = -lhdfs -lz

TARGETS = hdfsls hdfsread hdfslistblocks hdfsdumper

all: $(TARGETS)

hdfsls: hdfsls.o hdfscpp.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

hdfsread: hdfsread.o hdfscpp.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

hdfslistblocks: hdfslistblocks.o hdfscpp.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

# http://makepp.sourceforge.net/1.19/makepp_cookbook.html#Do%20you%20really%20need%20a%20library%3f
hdfsdumpreader_module.o: hdfsdumpreader.o hdfscpp.o
	$(LD) -r -o $@ $^

hdfsdumper: hdfsdumper.o hdfsdumpreader_module.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

.PHONY: clean all

clean:
	rm -fv *.o $(TARGETS)
