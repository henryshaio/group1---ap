#include "mjson.h"
#include "JSON_request_handle.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int Handle_Action(const char * InputBuf, int socket_fd)
{
	double action = mjson_find_number(InputBuf, strlen(InputBuf), "$.Action", -1);
	printf("Action is %d\n", (int)action);
	if(action == 2)
	{
		return Handle_Return_AP_Info(InputBuf, socket_fd);
	}
	else if(action == 5)
	{
		return Handle_Send_Connected_User_Number(InputBuf, socket_fd);
	}
	else if(action == 6)
	{
		return Handle_Send_User_Info(InputBuf, socket_fd);
	}
	else if(action == 7)
	{
		return Handle_Download_Config(InputBuf, socket_fd);
	}
	else
	{
		return -1;
	}
}
int Handle_Return_AP_Info(const char * InputBuf, int socket_fd)
{
	uint32_t AP_Traffic_Received, AP_Traffic_Transmit;
	double AP_Usage_Percent;

	//go get the infoooooo
	system("mkdir APRecord");
	system("cat /proc/stat | grep 'cpu ' /proc/stat | awk '{usage=($2+$4)*100/($2+$4+$5)} END {print usage}' > ./APRecord/cpu_usage_record");
	FILE * fp = fopen("./APRecord/cpu_usage_record", "r");
	if(!fp)
	{
		return -1;
	}
	fscanf(fp, "%lf", &AP_Usage_Percent);
	fclose(fp);

	char buf[500];
	fp = fopen("/proc/net/dev", "r");
	if(!fp)
	{
		return -1;
	}
	while(fscanf(fp, "%s", buf) == 1)
	{
		if(strncmp(buf, "eth0:", sizeof("eth0:")) == 0)
		{
			fscanf(fp, "%u %*u %*u %*u %*u %*u %*u %*u %u", &AP_Traffic_Received, &AP_Traffic_Transmit);
		}
	}
	const unsigned int KiB = 1024;
	printf("AP Received Traffic is %u(KiB)\n", AP_Traffic_Received/KiB);
	printf("AP Transmit Traffic is %u(KiB)\n", AP_Traffic_Transmit/KiB);
	printf("AP Usage Percent is %.2f\n", AP_Usage_Percent);

	char OutputBuf[1000];
        const int length = 1000;
        snprintf(OutputBuf, length, "{\n  \"Action\" : 2,\n  \"AP CPU Usage Percent\" : %.2f,\n  \"Network Received Traffic(KiB)\" : %u,\n  \"Network Transmit Traffic(KiB)\" : %u\n}\n", AP_Usage_Percent, AP_Traffic_Received/KiB, AP_Traffic_Transmit/KiB);
	return send(socket_fd, OutputBuf, strlen(OutputBuf)+1, 0);
}
int Handle_Send_Connected_User_Number(const char * InputBuf, int socket_fd)
{
	uint32_t Connected_User_Number;

	//go get the infoooooo
	printf("AP Connected Device Amount is %u\n", Connected_User_Number);


	char OutputBuf[1000];
        const int length = 1000;
        snprintf(OutputBuf, length, "{\n  \"Action\" : 5,\n  \"Connected User Number\" : %u\n}\n", Connected_User_Number);
	return send(socket_fd, OutputBuf, strlen(OutputBuf)+1, 0);

}
int Handle_Send_User_Info(const char * InputBuf, int socket_fd)
{
	char User_External_IP[20], Username[100];
	printf("User's External IP is %s\n", User_External_IP);
	printf("Username is %s\n", Username);

	char OutputBuf[1000];
        const int length = 1000;
        snprintf(OutputBuf, length, "{\n  \"Action\" : 6,\n  \"User Out IP\" : %s,\n  \"Username\" : %s\n}\n", User_External_IP, Username);
	return send(socket_fd, OutputBuf, strlen(OutputBuf)+1, 0);
}
int Handle_Download_Config(const char * InputBuf, int socket_fd)
{
	char Config_Location[200];
	printf("Config Archive Location is in %s\n", Config_Location);

	char OutputBuf[1000];
        const int length = 1000;
        snprintf(OutputBuf, length, "{\n  \"Action\" : 7,\n  \"Config Archive Location\" : %s\n}\n", Config_Location);
	return send(socket_fd, OutputBuf, strlen(OutputBuf)+1, 0);
}
