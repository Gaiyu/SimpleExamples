#!/bin/sh
PID=$1
IP_ADDR=$2
BR_NAME='br-container'
WAN_NAME='enp0s3'
GATE_WAY='172.18.0.1'
ip link add ${BR_NAME} type bridge
ip link set ${BR_NAME} up
ip addr add ${GATE_WAY}/24 dev ${BR_NAME}
ip link add veth${PID} type veth peer name veth1
ip link set veth${PID} up
ip link set veth${PID} master ${BR_NAME}
ip link set veth1 netns ${PID}

sudo bash -c 'echo 1 > /proc/sys/net/ipv4/ip_forward'
iptables -t nat -I POSTROUTING -o ${WAN_NAME} -j MASQUERADE

mkdir -p /var/run/netns
ln -sf /proc/${PID}/ns/net /var/run/netns/net-${PID}
ip netns exec net-${PID} ip link set veth1 up
ip netns exec net-${PID} ip addr add ${IP_ADDR}/24 dev veth1
ip netns exec net-${PID} ip route add default via ${GATE_WAY}
