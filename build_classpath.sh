#!/bin/bash
#
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

# Based on Hadoop's 0.18.3 src/c++/libhdfs/test/test-libhdfs.sh

function show_usage() {
	echo "Usage: $0 <HADOOP HOME DIR> <JAVA HOME>" >> /dev/stderr
}

function test_directory() {
	if ! [ -d $1 ]; then
		echo "Argument '$1' is not a directory" >> /dev/stderr
		show_usage;
		exit 1
	fi
}

if [ $# -ne 2 ]; then
	echo "Wrong number of arguments." >> /dev/stderr	
	show_usage;
	exit 1;
fi

HADOOP_HOME=$1
JAVA_HOME=$2
test_directory $HADOOP_HOME
test_directory $JAVA_HOME



# Manipulate HADOOP_CONF_DIR so as to include 
# HADOOP_HOME/conf/hadoop-default.xml too
# which is necessary to circumvent bin/hadoop
HADOOP_CONF_DIR=$HADOOP_CONF_DIR:$HADOOP_HOME/conf

# CLASSPATH initially contains $HADOOP_CONF_DIR
CLASSPATH="${HADOOP_CONF_DIR}"
CLASSPATH=${CLASSPATH}:$JAVA_HOME/lib/tools.jar

# for developers, add Hadoop classes to CLASSPATH
if [ -d "$HADOOP_HOME/build/classes" ]; then
  CLASSPATH=${CLASSPATH}:$HADOOP_HOME/build/classes
fi
if [ -d "$HADOOP_HOME/build/webapps" ]; then
  CLASSPATH=${CLASSPATH}:$HADOOP_HOME/build
fi
if [ -d "$HADOOP_HOME/build/test/classes" ]; then
  CLASSPATH=${CLASSPATH}:$HADOOP_HOME/build/test/classes
fi

# so that filenames w/ spaces are handled correctly in loops below
IFS=

# add libs to CLASSPATH
for f in $HADOOP_HOME/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done
for f in $HADOOP_HOME/lib/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done

for f in $HADOOP_HOME/lib/jsp-2.0/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done

# restore ordinary behaviour
unset IFS

# Print classpath...
echo $HADOOP_CONF_DIR:$CLASSPATH 
