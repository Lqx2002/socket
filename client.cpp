#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
 
using namespace std;
 
int main(int argc,char** argv)
{
	int clitfd;  //文件描述符
	struct sockaddr_in serv_addr;  //目的服务端地址结构体
 
	memset(&serv_addr,0,sizeof(serv_addr));
 
	if(argc!=3)
	{
		cout<<"Input error! Usage should be : "<<argv[0]<<"  xxx.xxx.xxx.xxx(ip)  1234(port)"<<endl;
		return 0;
	}
 
	if((clitfd = socket(AF_INET,SOCK_STREAM,0)) == -1)  //创建套接字
	{
		cout<<"creat socket failed : "<<strerror(errno)<<endl;
		return 0;
	}
        //将目的服务端的地址信息赋值给地址结构体
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
 
	cout<<"try to connect ... "<<endl;
 
        //通过套接字发起连接请求，成功后clitfd套接字则表示此次成功的连接
	if( connect(clitfd,(struct sockaddr*)& serv_addr,sizeof(serv_addr)) == -1)
	{
		cout<<"connet failed : "<<strerror(errno)<<endl;
		return 0;
	}
 
	cout<<"connect success !"<<endl;
 
	while(1)
	{
		char sdbuf[1024];
		char rvbuf[1024];
		int rdlen,sdlen,i=0;
		cout<<"(Client)send : ";
 
		while((sdbuf[i] = getchar()) != '\n')i++;
                if(i==0)continue; //防止客户端只发一个换行符，此时i=0，write不发送数据，服务端就不回回发数据，然后客户端就一直阻塞在read处。因此如果i=0，则直接重新输入
 
		sdlen = write(clitfd,sdbuf,i);  //向套接字中写入数据发送
        //可能会出现发送端把长度为sdlen的字符串分为多次发送，调用一次read就很有可能不能读取完全，就有以下两种方式进行读取：
        /*1. write了多少字节就读取多少字节长的字符串*/
		rdlen=0;
		while(rdlen<sdlen)//防止发送端将数据分开发送
		{	
			int rdcnt = read(clitfd,&rvbuf[rdlen],sizeof(rvbuf));
			if(rdcnt == -1)
			{
				perror(NULL);
				continue;
			}
			rdlen+=rdcnt;
		}
                if(rdlen)
                {
		    rvbuf[rdlen]='\0';
		    cout<<"(Client)recv : "<<rvbuf<<endl;
                }   
                else 
		{
	            cout<<"Server has closed ! "<<endl;
		    cout<<"Client will close..."<<endl;
		break;
		} 
 
 
        /*2. 用recv函数中的MSG_WAITALL参数，读到指定长度的数据才返回*/
        /*if(rdlen = recv(clitfd,&rvbuf[rdlen],sdlen,MSG_WAITALL))
		{	
			rvbuf[rdlen]='\0';
	
			cout<<"(Client)recv : "<<rvbuf<<endl;
		}
		else 
		{
			cout<<"Server has closed ! "<<endl;
			cout<<"Client will close..."<<endl;
			break;
		}*/
 
	}
 
	close(clitfd);
}
