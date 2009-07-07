

# Some ideas were taken from
# http://community.abiquo.com/display/ADP/Using+libhdfs

JAVA_HOME=/usr/lib/jvm/java-6-sun
JRE_PATH=${JAVA_HOME}/jre/lib/i386/
LIBJVM_PATH=${JRE_PATH}/server/:${JRE_PATH}/client/

HADOOP_HOME=/home/speed/hadoop/hadoop-64bits/hadoop/
HADOOP_CONF_DIR=${HADOOP_HOME}/conf
HDFS_LIB_DIR=${HADOOP_HOME}/libhdfs

export CLASSPATH=${HADOOP_CONF_DIR}:$(ls ${HADOOP_HOME}/hadoop*core*jar| head -1):$(find ${HADOOP_HOME}/lib -maxdepth 1 -name '*.jar' -print0| tr '\000' ':')

export LD_LIBRARY_PATH=${LIBJVM_PATH}:${HDFS_LIB_DIR}:${LD_LIBRARY_PATH}

