#!/bin/sh
PID=$1
SUB_SYS='cpu memory blkio devices freezer'
for line in $SUB_SYS
do
	echo $PID > /sys/fs/cgroup/${line}/tasks
done
rmdir /sys/fs/cgroup/cpuset/container
mkdir -p /sys/fs/cgroup/cpuset/container
echo 0 > /sys/fs/cgroup/cpuset/container/cpuset.cpus
echo 0 > /sys/fs/cgroup/cpuset/container/cpuset.mems
echo $PID > /sys/fs/cgroup/cpuset/container/tasks
echo $PID > /sys/fs/cgroup/cpuset/container/cgroup.procs
