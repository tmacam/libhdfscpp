# Copyright 2009 Tiago Alves Macambira
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# From Hadoop's libhdfs Makefile
#
# BEGIN User customizable settings. Edit to match your settings
#For amd64 use OS_ARCH=amd64 and N_BITS=64
OS_ARCH=i386
PLATFORM=linux
JAVA_HOME=/usr/lib/jvm/java-6-sun
N_BITS=32 

LIBHDFS_HOME=/home/tmacam/projects/libhdfs-0.18.3/

HADOOP_HOME=/home/tmacam/projects/hadoop-0.18.3/

# END User customizable setings.
#
# You probably don't need to touch the lines bellow. 

JAVA_INCLUDES = -m$(N_BITS) -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(PLATFORM)
JAVA_LDFLAGS = -L$(JAVA_HOME)/jre/lib/$(OS_ARCH)/server  -ljvm -m$(N_BITS)

LIBHDFS_INCLUDES = -I${LIBHDFS_HOME}/include
LIBHDFS_LDFLAGS = -L${LIBHDFS_HOME}/lib -lhdfs

CXXFLAGS =  -g -Wall -O0 -fPIC
CPPFLAGS =  $(LIBHDFS_INCLUDES) $(JAVA_INCLUDES)
LDFLAGS = $(LIBHDFS_LDFLAGS) $(JAVA_LDFLAGS)

TARGETS = libhdfscpp.so hdfsls hdfsread hdfslistblocks hdfsdumper LDLIBPATH.txt  CLASSPATH.txt libhdfscpp.pc

all: $(TARGETS)

libhdfscpp.so: hdfscpp.cpp hdfscpp.h filebuf.h
	$(CXX) -o $@ -shared $(CPPFLAGS) $(CXXFLAGS) $< 

hdfsls: hdfsls.o libhdfscpp.so
	$(CXX) $(LDFLAGS) $^ -o $@

hdfsread: hdfsread.o libhdfscpp.so
	$(CXX) $(LDFLAGS) $^ -o $@

hdfslistblocks: hdfslistblocks.o libhdfscpp.so
	$(CXX) $(LDFLAGS) $^ -o $@

# http://makepp.sourceforge.net/1.19/makepp_cookbook.html#Do%20you%20really%20need%20a%20library%3f
# WARNING WARNING WARNING WARNING: -lz is required for apps using this module
hdfsdumpreader_module.o: hdfsdumpreader.o hdfscpp.o
	$(LD) -r $^ -o $@ 

hdfsdumper: hdfsdumper.o hdfsdumpreader.o libhdfscpp.so
	$(CXX) $(LDFLAGS) -lz $^ -o $@

LDLIBPATH.txt:Makefile
	echo ${LIBHDFS_HOME}/lib:$(JAVA_HOME)/jre/lib/$(OS_ARCH)/server:`pwd` > .$@.tmp
	mv .$@.tmp $@

CLASSPATH.txt: Makefile build_classpath.sh
	./build_classpath.sh $(HADOOP_HOME) $(JAVA_HOME) > .$@.tmp
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
