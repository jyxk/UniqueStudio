#include <sys/types.h>
#include <sys/socket.h>  
#include <sys/stat.h>
#include <netinet/in.h>  
#include <arpa/inet.h> 
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <netdb.h>
#include "hosts.h"
#define DEF_DNS_ADDRESS "119.29.29.29"	//外部DNS服务器地址
#define LOCAL_ADDRESS "127.0.0.1"		//本地DNS服务器地址
#define DNS_PORT 53						//进行DNS服务的53端口
#define BUF_SIZE 512
#define LENGTH 65
#define AMOUNT 300
#define NOTFOUND (-1)
#define SOCKET_ERROR (-1)
using namespace std;
//DNS报文首部
typedef struct DNSHeader
{
    unsigned short ID;
    unsigned short Flags;
    unsigned short QuestNum;
    unsigned short AnswerNum;
    unsigned short AuthorNum;
    unsigned short AdditionNum;
} DNSHDR, *pDNSHDR;

//DNS域名解析表结构
typedef struct translate
{
	string IP;						//IP地址
	string domain;					//域名
} Translate;

//ID转换表结构
typedef struct IDChange
{
	unsigned short oldID;			//原有ID
	bool done;						//标记是否完成解析
	sockaddr_in client;				//请求者套接字地址
} IDTransform;

Translate DNS_table[AMOUNT];		//DNS域名解析表
IDTransform IDTransTable[AMOUNT];	//ID转换表
int IDcount = 0;					//转换表中的条目个数
string url;					//域名
Hosts *hosts = new Hosts();

int hostname_to_ip(const char * hostname, char* ip) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return 1;
}

//函数：获取域名解析表
int GetTable(char *tablePath)
{
	int i=0, j, pos=0;
	string table[AMOUNT];

	ifstream infile(tablePath, ios::in);	//以读入方式打开文本文件

	if(!infile) {
		printf("read table failed\n");
		exit(1);
	}

	//每次从文件中读入一行，直至读到文件结束符为止
	while (getline(infile, table[i]) && i < AMOUNT)
		i++;

	if (i == AMOUNT-1)
		cout << "The DNS table memory is full. " << endl;

	for (j = 0; j < i-1; j++) {
		pos = table[j].find(' ');
		if (pos > table[j].size())
			cout << "The record is not in a correct format. " << endl;
		else {
			string ip = table[j].substr(0, pos);
			string domain = table[j].substr(pos+1);
			cout << ip << domain << endl;
			hosts->Update(ip.c_str(), domain.c_str());
		}
	}

	infile.close();		//关闭文件
	cout << "Load records succeed. " <<i-1<< endl;

	return i-1;			//返回域名解析表中条目个数
}

//函数：获取DNS请求中的域名
void GetUrl(char *recvbuf, int recvnum)
{
	char urlname[LENGTH];
	int i = 0, j, k = 0;

	//memset(url, 0, LENGTH);
	memcpy(urlname, &(recvbuf[sizeof(DNSHDR)]), recvnum-16);	//获取请求报文中的域名表示

	size_t len = strlen(urlname);
	
	//域名转换
	while (i < len) {
		if (urlname[i] > 0 && urlname[i] <= 63)
			for (j = urlname[i], i++; j > 0; j--, i++)
				url += urlname[i];
		
		if (urlname[i] != 0)
			url += '.';

	}

	url.append("\0");
}

//函数：判断是否在表中找到DNS请求中的域名，找到返回下标
int IsFind(char* url, int num)
{
	int find = NOTFOUND;
	char* domain;

	for (int i = 0; i < num; i++) {
		domain = (char *)DNS_table[i].domain.c_str();
		if (strcmp(domain, url) == 0) {	//找到
			find = i;
			break;
		}
	}

	return find;
}

//函数：将请求ID转换为新的ID，并将信息写入ID转换表中
unsigned short RegisterNewID (unsigned short oID, sockaddr_in temp, bool ifdone)
{
	srand(time(NULL));
	IDTransTable[IDcount].oldID = oID;
	IDTransTable[IDcount].client = temp;
	IDTransTable[IDcount].done  = ifdone;
	IDcount++;

	return (unsigned short)(IDcount-1);	//以表中下标作为新的ID
}

//函数：打印 
void DisplayInfo(unsigned short newID, bool find)
{
	
	//打印转换后新的ID
	cout.setf(ios::left);
	cout << setiosflags(ios::left) << setw(4) << setfill(' ') << newID;
	cout << "    ";

	//在表中没有找到DNS请求中的域名
	if (!find)
	{   
		//中继功能
		cout.setf(ios::left);
		cout << setiosflags(ios::left) << setw(6) << setfill(' ') << "中继";
		cout << "    ";
		//打印域名
		cout.setf(ios::left);
		cout << setiosflags(ios::left) << setw(20) << setfill(' ') << url;
		cout << "    ";
		//打印IP
		cout.setf(ios::left);
		cout << setiosflags(ios::left) << setw(20) << setfill(' ') << endl;
	}

	//在表中找到DNS请求中的域名
	else {
		//检索结果为普通IP地址，则向客户返回这个地址
			//服务器功能
			cout.setf(ios::left);
		    cout << setiosflags(ios::left) << setw(6) << setfill(' ') << "服务器";
		    cout << "    ";
			//打印域名
			cout.setf(ios::left);
		    cout << "*" << setiosflags(ios::left) << setw(19) << setfill(' ') << url;
		    cout << "    ";
			//打印IP
			cout.setf(ios::left);
		    cout << setiosflags(ios::left) << setw(20) << setfill(' ') << hosts->result_index()[url] << endl;
	}
}

int DateHandle() {
			GetUrl(recvbuf, iRecv);				//获取域名
			bool find = true;		//在域名解析表中查找
            cout << "*"<<result[url]<<"*"<<url<<"*" << endl;

            if (result[url].empty())
                find = false;

			//在域名解析表中没有找到
			if (!find) {
				//ID转换
				auto pID = (unsigned short *)malloc(sizeof(unsigned short));
				memcpy(pID, recvbuf, sizeof(unsigned short));
				unsigned short nID = htons(RegisterNewID(ntohs(*pID), client_name, false));
				memcpy(recvbuf, &nID, sizeof(unsigned short));

				//打印
				DisplayInfo(ntohs(nID), find);

				//把recvbuf转发至指定的外部DNS服务器
				iSend = sendto(socket_server, recvbuf, iRecv, 0, (sockaddr*)&server_name, sizeof(server_name));
				if (iSend == SOCKET_ERROR) {
					printf("sendto failed.\n");
					continue;
				}
				else if (iSend == 0)
					break;

				free(pID);	//释放动态分配的内存

				//接收来自外部DNS服务器的响应报文
				iRecv = recvfrom(socket_server, recvbuf, sizeof(recvbuf), 0, (sockaddr*)&client_name, &iLen_cli);

				//ID转换
				pID = (unsigned short *)malloc(sizeof(unsigned short));
				memcpy(pID, recvbuf, sizeof(unsigned short));
				int m = ntohs(*pID);
				unsigned short oID = htons(IDTransTable[m].oldID);
				memcpy(recvbuf, &oID, sizeof(unsigned short));
				IDTransTable[m].done = true;

				//从ID转换表中获取发出DNS请求者的信息
				client_name = IDTransTable[m].client;

				//把recvbuf转发至请求者处
				iSend = sendto(socket_local, recvbuf, iRecv, 0, (sockaddr*)&client_name, sizeof(client_name));
				char *target_ip = (char *)malloc(sizeof(char)*16);
				hostname_to_ip(url.c_str(), target_ip);
				result[url] = target_ip;
				free(target_ip);
				if (iSend == SOCKET_ERROR) {
					printf("sendto failed.1\n");
					continue;
				}
				else if (iSend == 0)
					break;

				free(pID);	//释放动态分配的内存
			}

			//在域名解析表中找到
			else {	
				//获取请求报文的ID
				auto pID = (unsigned short *)malloc(sizeof(unsigned short));
				memcpy(pID, recvbuf, sizeof(unsigned short));

				//转换ID
				unsigned short nID = RegisterNewID(ntohs(*pID), client_name, false);

				//打印 
				DisplayInfo(nID, find);

				//构造响应报文返回
				memcpy(sendbuf, recvbuf, iRecv);						//拷贝请求报文
				unsigned short a = htons(0x8180);
				memcpy(&sendbuf[2], &a, sizeof(unsigned short));		//修改标志域

				//修改回答数域
				a = htons(0x0001);	//服务器功能：回答数为1
				memcpy(&sendbuf[6], &a, sizeof(unsigned short));
				int curLen = 0;

				//构造DNS响应部分
				char answer[16] = {};
				unsigned short Name = htons(0xc00c);
				memcpy(answer, &Name, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned short TypeA = htons(0x0001);
				memcpy(answer+curLen, &TypeA, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned short ClassA = htons(0x0001);
				memcpy(answer+curLen, &ClassA, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned int timeLive = htonl(0x007b);
				memcpy(answer+curLen, &timeLive, sizeof(unsigned int));
				curLen += sizeof(unsigned int);

				unsigned short IPLen = htons(0x0008);
				memcpy(answer+curLen, &IPLen, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned int IP =(unsigned int) inet_addr(result[url].c_str());
				cout << curLen << endl;
				//sprintf(answer+curLen, "%ud", IP);
				memcpy(answer+curLen, &IP, sizeof(unsigned int));
				curLen += sizeof(unsigned int);
				curLen += iRecv;

				//请求报文和响应部分共同组成DNS响应报文存入sendbuf
				memcpy(sendbuf+iRecv, answer, curLen);

				//发送DNS响应报文
				iSend = sendto(socket_local, sendbuf, curLen, 0, (sockaddr*)&client_name, sizeof(client_name));
				if (iSend == SOCKET_ERROR) {
					cout << "sendto Failed 2" << endl;
					exit();
				}
				else if (iSend == 0)
					break;
			
				free(pID);		//释放动态分配的内存
			}

			url.clear();
}


int main(int argc, char** argv) { 

    int  socket_server, socket_local;				//本地DNS和外部DNS两个套接字
    struct sockaddr_in server_name, client_name, local_name;	//本地DNS、外部DNS和请求端三个网络套接字地址
    char sendbuf[BUF_SIZE];
    char recvbuf[BUF_SIZE]; 
    char tablePath[100];
    char outerDns[16];
    socklen_t iLen_cli, iSend, iRecv;
    int num;
	unordered_map<string, string> &result = hosts->result_index();

	if (argc == 1) {
		strcpy(outerDns, DEF_DNS_ADDRESS);
		strcpy(tablePath, "/home/jyxk/Code/FreshMan/third/hosts");
	}

	else if (argc == 2) {
		strcpy(outerDns, argv[1]);
		strcpy(tablePath, "/home/jyxk/Code/FreshMan/third/hosts");
	}

	else if (argc == 3) {
		strcpy(outerDns, argv[1]);
		strcpy(tablePath, argv[2]);
	}

	num = GetTable(tablePath);						//获取域名解析表
    //hosts->StatHosts(tablePath);

	for (int i=0; i < AMOUNT; i++) {				//初始化ID转换表
		IDTransTable[i].oldID = 0;
		IDTransTable[i].done  = false;
		memset(&(IDTransTable[i].client), 0, sizeof(sockaddr_in));
	}

	//创建本地DNS和外部DNS套接字
    socket_server = socket(AF_INET, SOCK_DGRAM, 0);
	socket_local = socket(AF_INET, SOCK_DGRAM, 0);

	//设置本地DNS和外部DNS两个套接字
	local_name.sin_family = AF_INET;
	local_name.sin_port = htons(DNS_PORT);
	local_name.sin_addr.s_addr = inet_addr(LOCAL_ADDRESS);

	server_name.sin_family = AF_INET;
	server_name.sin_port = htons(DNS_PORT);
	server_name.sin_addr.s_addr = inet_addr(outerDns);

	//绑定本地DNS服务器地址
	if (-1 == bind(socket_local, (sockaddr*)&local_name, sizeof(local_name))) {
		printf("Binding Port 53 failed\n");
		close(socket_local);
		exit(1);
	}
	else
		printf("Binding Port 53 succeed.\n");

	//本地DNS中继服务器的具体操作
	while (true) {
		iLen_cli = sizeof(client_name);
        memset(recvbuf, 0, BUF_SIZE);

		//接收DNS请求
		iRecv = recvfrom(socket_local, recvbuf, sizeof(recvbuf), 0, (sockaddr*)&client_name, &iLen_cli);

		if (iRecv == SOCKET_ERROR) {
			printf("Recvfrom failed.\n");
			continue;
		}
		else if (iRecv == 0) {
			break;
		}
		else {
			GetUrl(recvbuf, iRecv);				//获取域名
			bool find = true;		//在域名解析表中查找
            cout << "*"<<result[url]<<"*"<<url<<"*" << endl;

            if (result[url].empty())
                find = false;

			//在域名解析表中没有找到
			if (!find) {
				//ID转换
				auto pID = (unsigned short *)malloc(sizeof(unsigned short));
				memcpy(pID, recvbuf, sizeof(unsigned short));
				unsigned short nID = htons(RegisterNewID(ntohs(*pID), client_name, false));
				memcpy(recvbuf, &nID, sizeof(unsigned short));

				//打印
				DisplayInfo(ntohs(nID), find);

				//把recvbuf转发至指定的外部DNS服务器
				iSend = sendto(socket_server, recvbuf, iRecv, 0, (sockaddr*)&server_name, sizeof(server_name));
				if (iSend == SOCKET_ERROR) {
					printf("sendto failed.\n");
					continue;
				}
				else if (iSend == 0)
					break;

				free(pID);	//释放动态分配的内存

				//接收来自外部DNS服务器的响应报文
				iRecv = recvfrom(socket_server, recvbuf, sizeof(recvbuf), 0, (sockaddr*)&client_name, &iLen_cli);

				//ID转换
				pID = (unsigned short *)malloc(sizeof(unsigned short));
				memcpy(pID, recvbuf, sizeof(unsigned short));
				int m = ntohs(*pID);
				unsigned short oID = htons(IDTransTable[m].oldID);
				memcpy(recvbuf, &oID, sizeof(unsigned short));
				IDTransTable[m].done = true;

				//从ID转换表中获取发出DNS请求者的信息
				client_name = IDTransTable[m].client;

				//把recvbuf转发至请求者处
				iSend = sendto(socket_local, recvbuf, iRecv, 0, (sockaddr*)&client_name, sizeof(client_name));
				char *target_ip = (char *)malloc(sizeof(char)*16);
				hostname_to_ip(url.c_str(), target_ip);
				result[url] = target_ip;
				free(target_ip);
				if (iSend == SOCKET_ERROR) {
					printf("sendto failed.1\n");
					continue;
				}
				else if (iSend == 0)
					break;

				free(pID);	//释放动态分配的内存
			}

			//在域名解析表中找到
			else {	
				//获取请求报文的ID
				auto pID = (unsigned short *)malloc(sizeof(unsigned short));
				memcpy(pID, recvbuf, sizeof(unsigned short));

				//转换ID
				unsigned short nID = RegisterNewID(ntohs(*pID), client_name, false);

				//打印 
				DisplayInfo(nID, find);

				//构造响应报文返回
				memcpy(sendbuf, recvbuf, iRecv);						//拷贝请求报文
				unsigned short a = htons(0x8180);
				memcpy(&sendbuf[2], &a, sizeof(unsigned short));		//修改标志域

				//修改回答数域
				a = htons(0x0001);	//服务器功能：回答数为1
				memcpy(&sendbuf[6], &a, sizeof(unsigned short));
				int curLen = 0;

				//构造DNS响应部分
				char answer[16] = {};
				unsigned short Name = htons(0xc00c);
				memcpy(answer, &Name, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned short TypeA = htons(0x0001);
				memcpy(answer+curLen, &TypeA, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned short ClassA = htons(0x0001);
				memcpy(answer+curLen, &ClassA, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned int timeLive = htonl(0x007b);
				memcpy(answer+curLen, &timeLive, sizeof(unsigned int));
				curLen += sizeof(unsigned int);

				unsigned short IPLen = htons(0x0008);
				memcpy(answer+curLen, &IPLen, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned int IP =(unsigned int) inet_addr(result[url].c_str());
				cout << curLen << endl;
				//sprintf(answer+curLen, "%ud", IP);
				memcpy(answer+curLen, &IP, sizeof(unsigned int));
				curLen += sizeof(unsigned int);
				curLen += iRecv;

				//请求报文和响应部分共同组成DNS响应报文存入sendbuf
				memcpy(sendbuf+iRecv, answer, curLen);

				//发送DNS响应报文
				iSend = sendto(socket_local, sendbuf, curLen, 0, (sockaddr*)&client_name, sizeof(client_name));
				if (iSend == SOCKET_ERROR) {
					cout << "sendto Failed 2" << endl;
					continue;
				}
				else if (iSend == 0)
					break;
			
				free(pID);		//释放动态分配的内存
			}

			url.clear();
		}
	}

    close(socket_server);	//关闭套接字
	close(socket_local);


    return 0;
}