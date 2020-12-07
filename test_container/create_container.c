#define _GNU_SOURCE
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/syscall.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/mman.h>

static const int STACK_SIZE = 1024 * 1024;
static const char* script_path = "/home/gaiyu/test_container/pid_cgroup.sh";

static int pivot_root(const char *new_root, const char *put_old)
{
	return syscall(SYS_pivot_root, new_root, put_old);
}

void set_map(char* file, int inside_id, int outside_id, int len)
{
    FILE* mapfd = fopen(file, "w");
    if (NULL == mapfd) {
        perror("open file error");
        return;
    }
    fprintf(mapfd, "%d %d %d", inside_id, outside_id, len);
    fclose(mapfd);
}

void set_uid_map(pid_t pid, int inside_id, int outside_id, int len)
{
    char file[256];
    sprintf(file, "/proc/%d/uid_map", pid);
    set_map(file, inside_id, outside_id, len);
}

void set_gid_map(pid_t pid, int inside_id, int outside_id, int len)
{
    char file[256];
    sprintf(file, "/proc/%d/gid_map", pid);
    set_map(file, inside_id, outside_id, len);
}

int exec_container(void * arg)
{
	char ** args =arg;
	char * new_root = args[0];
	const char* put_old = "/old_rootfs";
	char path[PATH_MAX];

	if ((1 == mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL))
		|| (-1 == mount(new_root, new_root, NULL, MS_BIND, NULL)))
		exit(EXIT_FAILURE);

	snprintf(path, sizeof(path), "%s/%s", new_root, put_old);
	printf("path = %s\n", path);
	printf("new_root = %s\n", new_root);
	printf("--------------------\n");

	char proc_path[PATH_MAX];
	snprintf(proc_path, sizeof(proc_path), "%s/proc", new_root);
	umount2("/proc", MNT_DETACH);
	mount("proc", proc_path, "proc", 0, NULL);

	if ((-1 == mkdir(path, 0777))
		|| (-1 == pivot_root(new_root, path))
		|| (-1 == chdir("/"))
		|| (-1 == umount2(put_old, MNT_DETACH))
		|| (-1 == rmdir(put_old)))
		exit(EXIT_FAILURE);
	return execv(args[1], &args[1]);
}

int main(int argc, char *argv[])
{
	char *stack = mmap(NULL, STACK_SIZE
			, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
	if (MAP_FAILED == stack)
		exit(EXIT_FAILURE);

	const int gid = getgid();
	const int uid = getuid();
	int pid = clone(exec_container, stack + STACK_SIZE
			, CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUSER | CLONE_NEWNET | CLONE_NEWIPC | CLONE_NEWUTS | SIGCHLD
			, &argv[1]);
	if (-1 == pid)
		exit(EXIT_FAILURE);

	set_uid_map(pid, 0, uid, 1);
	set_gid_map(pid, 0, gid, 1);

	char script[256];
	sprintf(script , "sudo %s %d", script_path, pid);
    printf("%s\n", script);
    printf("--------------------\n");

	if (wait(NULL) == -1)
		exit(EXIT_FAILURE);
	exit(EXIT_SUCCESS);
}
