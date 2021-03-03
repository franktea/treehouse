#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "ae.h"
#include "anet.h"

// 写数据到客户端的回调函数
void writeToClient(aeEventLoop *loop, int fd, void *clientdata, int mask)
{
    char *buffer = clientdata;
    printf("recv client [%d] data: %s\n", fd, buffer);
    // 未对write的返回值做判断，可以认为是bug，文章后面有说明
    write(fd, buffer, strlen(buffer));
    free(buffer);
    aeDeleteFileEvent(loop, fd, mask);
}

// 从客户端读数据的回调函数
void readFromClient(aeEventLoop *loop, int fd, void *clientdata, int mask)
{
    int buffer_size = 1024;
    char *buffer = calloc(buffer_size, sizeof(char));
    int size = read(fd, buffer, buffer_size);
    if (size <= 0)
    {
      printf("Client disconnected\n");
      free(buffer);
      aeDeleteFileEvent(loop, fd, AE_READABLE);
      return; 
    }
    aeCreateFileEvent(loop, fd, AE_WRITABLE, writeToClient, buffer);
}

// 接手新连接的回调函数
void acceptTcpHandler(aeEventLoop *loop, int fd, void *clientdata, int mask)
{
    int client_port;
    char client_ip[128];
    // 接手一个新的连接
    int client_fd = anetTcpAccept(NULL, fd, client_ip, 128, &client_port);
    printf("Accepted %s:%d\n", client_ip, client_port);

    // 将fd设置为非阻塞
    anetNonBlock(NULL, client_fd);

    // 注册AE_READABLE，从该链接读取数据
    int ret = aeCreateFileEvent(loop, client_fd, AE_READABLE, readFromClient, NULL);
    assert(ret != AE_ERR);
}

int main()
{
    // 创建事件循环
    aeEventLoop *loop = aeCreateEventLoop(1024);
    
    // 打开监听端口的文件
    int listen_fd = anetTcpServer(NULL, 8000, "0.0.0.0", 0);
    assert(listen_fd != ANET_ERR);

    // 将listen_fd注册到loop
    int ret = aeCreateFileEvent(loop, listen_fd, AE_READABLE, acceptTcpHandler, NULL);
    assert(ret != AE_ERR);

    // 启动事件循环
    aeMain(loop);
    
    // 回收内存
    aeDeleteEventLoop(loop);
}
