#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void mk_dir(void);

int main(void)
{
	int old_fd = -1, cur_fd = -1, new_fd = -1, status = -1;
	off_t len = -1;
	char path[30] = {0}, file_name[16] = {0};
	pid_t pid_num = -1;
	
	mk_dir();
	
	while(1)
	{
		cur_fd = open("./verinfo/cur_soft", O_RDONLY|O_CREAT, S_IRUSR|S_IWUSR);
	
		if(cur_fd < 0)
		{
			perror("LOADER");
			return;
		}
		
		len = read(cur_fd, file_name, sizeof(file_name));
		
		if(len < 0)
		{
			close(cur_fd);
			perror("LOADER");
			return;
		}
		close(cur_fd);
		
		sprintf(path, "./software/%s", file_name);
		
		pid_num = fork();
		
		if(pid_num < 0)
		{
			perror("LOADER");
		}else if(pid_num == 0)
		{
			execl(path, NULL, NULL);
			printf("software error!\n");
			exit(-1);
		}else
		{
			wait(&status);
			printf("software reset! %d\n", status);
			sleep(1);
		}
	}
}

void mk_dir(void)
{
	int old_fd = -1, cur_fd = -1, new_fd = -1;
		
	if(access("./verinfo", F_OK) < 0)		//存放软件名称和版本信息的文件夹
		mkdir("./verinfo", 0777);

	if(access("./verinfo/cur_soft", F_OK) < 0)
	{
		cur_fd = creat("./verinfo/cur_soft", S_IRUSR|S_IWUSR);
		write(cur_fd, "rru", 3);
		lseek(cur_fd, 16, SEEK_SET);
		write(cur_fd, "TLSWV1.0", 8);
		close(cur_fd);
	}
	if(access("./verinfo/old_soft", F_OK) < 0)
	{
		old_fd = creat("./verinfo/old_soft", S_IRUSR|S_IWUSR);
		write(old_fd, "rru", 3);
		lseek(old_fd, 16, SEEK_SET);
		write(old_fd, "TLSWV1.0", 8);
		close(old_fd);
	}
	if(access("./verinfo/new_soft", F_OK) < 0)
	{
		new_fd = creat("./verinfo/new_soft", S_IRUSR|S_IWUSR);
		write(new_fd, "rru", 3);
		lseek(new_fd, 16, SEEK_SET);
		write(new_fd, "TLSWV1.0", 8);
		close(new_fd);
	}

	if(access("./verinfo/cur_firm", F_OK) < 0)
	{
		cur_fd = creat("./verinfo/cur_firm", S_IRUSR|S_IWUSR);
		write(cur_fd, "fpga", 4);
		lseek(cur_fd, 16, SEEK_SET);
		write(cur_fd, "TLFWV1.0", 8);
		close(cur_fd);
	}
	if(access("./verinfo/old_firm", F_OK) < 0)
	{
		old_fd = creat("./verinfo/old_firm", S_IRUSR|S_IWUSR);
		write(old_fd, "fpga", 4);
		lseek(old_fd, 16, SEEK_SET);
		write(old_fd, "TLFWV1.0", 8);
		close(old_fd);
	}
	if(access("./verinfo/new_firm", F_OK) < 0)
	{
		new_fd = creat("./verinfo/new_firm", S_IRUSR|S_IWUSR);
		write(new_fd, "fpga", 4);
		lseek(new_fd, 16, SEEK_SET);
		write(new_fd, "TLFWV1.0", 8);
		close(new_fd);
	}
	
	if(access("./cpriinfo", F_OK) < 0)		//存放cpri信息的文件夹
		mkdir("./cpriinfo", 0777);
	
	if(access("./log", F_OK) < 0)			//存放log日志的文件夹
		mkdir("./log", 0777);
	
	if(access("./software", F_OK) < 0)		//存放软件的文件夹
		mkdir("./software", 0777);
	
	if(access("./firmware", F_OK) < 0)		//存放固件的文件夹
		mkdir("./firmware", 0777);
}
