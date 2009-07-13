
# From Hadoop's libhdfs Makefile
OS_ARCH=i386
PLATFORM=linux
JAVA_HOME=/usr/lib/jvm/java-6-sun

JAVA_INCLUDES = -m32 -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(PLATFORM)
JAVA_LDFLAGS = -L$(JAVA_HOME)/jre/lib/$(OS_ARCH)/server  -ljvm -shared -m32 -Wl,-x 

HADOOP_HOME=/home/speed/hadoop/hadoop-64bits/hadoop
HADOOP_INCLUDES = -I${HADOOP_HOME}/src/c++/libhdfs

CXXFLAGS   =  $(HADOOP_INCLUDES) $(JAVA_INCLUDES) -g -O0
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

hdfsdumper: hdfsdumper.o hdfsdumpreader.o hdfscpp.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

.PHONY: clean all

clean:
	rm -fv *.o $(TARGETS)
