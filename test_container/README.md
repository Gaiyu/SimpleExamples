## 编译
###### 编译容器引擎例子
使用gcc编译即可，只依赖C库。
```
gcc ./create_container.c -o create_container
```
或者静态链接
```
gcc --static ./create_container.c -o create_container
```
###### 编译测试容器代码，此代码占满CPU。
```
go build -o cpu100 main.go
```
# 准备测试环境
###### 准备测试容器根文件系统，可以直接从docker镜像中导出。
```
mkdir -p rootfs ; docker export $(docker create alpine:3) | tar -C rootfs -xf  -
```
###### 将测测试程序cpu100拷贝到容器根文件系统bin目录下。
```
cp cpu100 ./rootfs/bin
chmod 777 ./rootfs
```
# 运行
###### 使用./rootfs做为容器根文件系统启动容器，并用sh做为容器init进程（PID == 1）。
```
sudo ./create_container ./rootfs/ /bin/busybox sh
```
###### 使用pid_cgroup.sh为启动的容器注册控制组，限制容器只能使用0号CPU内核。容器进程PID会在create_container程序运行后打印到终端上。
```
./pid_cgroup.sh <PID of container>
```
###### 容器默认网关为172.18.0.1，通过setup_net.sh为容器的network命名空间配置网络。
例如：配置容器IP地址为172.18.0.3
```
./setup_net.sh <PID of container> 172.18.0.3
```
# 测试
###### 在容器的shell中运行cpu100，可以看到容器只有1个CPU。
也可以通过cat /proc/<PID of container>/status | grep Cpus_allowed_list来查看
