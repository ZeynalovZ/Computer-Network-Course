#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "general.h"
#include <string.h>

#include <chrono>

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

struct statusCodes
{
  char* statusCode;
  char* message;
};

struct existingURLS
{
  char* url;
  char* htmlPage;
};


static statusCodes OKCode = {"200", "OK"};
static statusCodes NotFoundCode = {"404", "Not Found"};
static statusCodes IternalErrorCode = {"500", "Iternal error"};



static existingURLS existingURI[3] = {{"/test.html", "test.html"},{"/mm.html", "mm.html"},{"klimat05.html", "<div><h1> hello world</h1></div>"}};



string getInfoFromFile(char* fileName)
{
  string fileRes = "";
  string line;
  std:ifstream in(fileName);
  if (in.is_open())
  {
    while (getline(in, line))
    {
        fileRes.append(line);
    }

    in.close();
  }
  return fileRes;
}

void saveUserStatistic(string userId, char* url)
{
  std:ofstream of;
  of.open(userId + ".txt", ios_base::app);
  if (of.is_open())
  {
    of << userId + " visited page: " + url << endl;

    of.close();
  }
}

void perror_and_exit(char *s, int exit_code)
{
	perror(s);
	exit(exit_code);
}

string generateResponseMessage(char* headers, int pageId)
{
  string resultMsg = "";
  statusCodes statusCode;
  if (pageId != -1)
  {
    statusCode = OKCode;
  }
  else
  {
    statusCode = NotFoundCode;
  }
  resultMsg.append(HttpVersion);
  resultMsg.append(" ");
  resultMsg.append(statusCode.statusCode);
  resultMsg.append(" ");
  resultMsg.append(statusCode.message);
  resultMsg.append("\r\n");
  resultMsg.append("Connection: closed\r\n");
  resultMsg.append("Content-Type: text/html; charset=UTF-8\r\n");
  resultMsg.append("\r\n");
  if (pageId != -1)
  {
    string fileRes = getInfoFromFile(existingURI[pageId].htmlPage);
    if (fileRes != "")
      resultMsg.append(fileRes);
  }
  return resultMsg;
}


string handleRequestMessage(char * innerMessage)
{
  char* method = strtok(innerMessage, " ");
  char* url = strtok(NULL, " ");
  char* httpVersion = strtok(NULL, "\r\n");
  char* userName = strtok(NULL, "\r\n");
  char* HostName = strtok(NULL, "\r\n");
  cout << "+++" << userName << "+++" << endl;
  char *Name = strstr(userName, ": ");
  
  if (strcmp(method, "GET") == 0)
  {
    int pageId = -1;
    for (int i = 0; i < 3; i++)
    {
      if (strcmp(url, existingURI[i].url) == 0)
      {
        saveUserStatistic((string)(Name + 2), url);
        pageId = i;
        break;
      }
    }
    char res[MESSAGE_LEN];
    //return "asd";
    
    return generateResponseMessage(innerMessage, pageId);
  }
  else
  {
    return "It's not Get";
  }
  
}


int main()
{
    int sock, listener;
    struct sockaddr_in addr, client_addr;
    char *buf = (char*)malloc(MESSAGE_LEN);
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
		perror_and_exit("socket()", 1);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		perror_and_exit("bind()", 2);

    listen(listener, 10);
  	printf("Server is listening on %s:%d...\n", ServIp, Port);
    
    while(1)
    {
		    socklen_t cli_addr_size = sizeof(client_addr);
        sock = accept(listener, (struct sockaddr*) &client_addr, &cli_addr_size);
        if(sock < 0)
			  perror_and_exit("accept()", 3);

        bytes_read = recv(sock, buf, MESSAGE_LEN, 0);
        buf[bytes_read] = '\0';
        char tst[MESSAGE_LEN];
        strcpy(tst, buf);
        string res = handleRequestMessage(tst);
        //cout <<"test;" << res << endl;
        const char * msg = res.c_str();
        printf("Client's message: %s\n", buf);
        if(bytes_read <= 0) break;
        printf("%s\n", msg);
        send(sock, msg, strlen(msg), 0);
        cout << "ok!" << endl;
        close(sock);
    }
    
    return 0;
}