

# Some ideas were taken from
# http://community.abiquo.com/display/ADP/Using+libhdfs

export OS_NAME=linux
export N_BITS=64
export OS_ARCH=amd64
export JAVA_HOME=/usr/lib/jvm/java-6-sun

JRE_PATH=${JAVA_HOME}/jre/lib/${OS_ARCH}/
LIBJVM_PATH=${JRE_PATH}/server/:${JRE_PATH}/client/

export HADOOP_HOME=/home/speed/hadoop/hadoop-64bits/hadoop/
export HADOOP_CONF_DIR=${HADOOP_HOME}/conf
HDFS_LIB_DIR=/home/speed/tmacam/hadoop-64bits/hadoop/libhdfs64

export CLASSPATH=${HADOOP_CONF_DIR}:$(ls ${HADOOP_HOME}/hadoop*core*jar| head -1):$(find ${HADOOP_HOME}/lib -maxdepth 1 -name '*.jar' -print0| tr '\000' ':')

export LD_LIBRARY_PATH=${LIBJVM_PATH}:${HDFS_LIB_DIR}:${LD_LIBRARY_PATH}
