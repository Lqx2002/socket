#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
 
#define SERV_PORT 8888        //服务器端口
#define SERV_IP "127.1.1.1"   //服务器ip
 
using namespace std;
 
int main(int argc,char** argv)
{
	int servfd,clitfd;   //创建两个文件描述符，servfd为监听套接字，clitfd用于数据传输
	struct sockaddr_in serv_addr,clit_addr; //创建地址结构体，分别用来存放服务端和客户端的地址信息
 
	memset(&serv_addr,0,sizeof(serv_addr));  //初始化
	memset(&clit_addr,0,sizeof(clit_addr));  //初始化
 
	if((servfd = socket(AF_INET,SOCK_STREAM,0)) == -1)  //创建套接字
	{
		cout<<"creat socket failed : "<<strerror(errno)<<endl;//如果出错则打印错误
		return 0;
	}
        
        //给服务端的地址结构体赋值
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT); //将主机上的小端字节序转换为网络传输的大端字节序（如果主机本身就是大端字节序就不用转换了）
	serv_addr.sin_addr.s_addr = inet_addr(SERV_IP); //将字符串形式的ip地址转换为点分十进制格式的ip地址
 
        //绑定地址信息到监听套接字上，第二个参数强转是因为形参类型为sockaddr ，而实参类型是sockaddr_in 型的
	if(bind(servfd,(sockaddr *)& serv_addr,sizeof(serv_addr)) == -1)
	{
		cout<<"bind failed : "<<strerror(errno)<<endl;
		return 0;
	}
        
        //将servfd套接字置为监听状态
	if(listen(servfd,1024) == -1)
	{
		cout<<"listen failed : "<<strerror(errno)<<endl;
		return 0;
	}
 
	cout<<"Init Success ! "<<endl;
	cout<<"ip : "<<inet_ntoa(serv_addr.sin_addr)<<"  port : "<<ntohs(serv_addr.sin_port)<<endl;
	cout<<"Waiting for connecting ... "<<endl;
 
	socklen_t clit_size = 0; //用于accept函数中保存客户端的地址结构体大小
 
        //accept成功后，clitfd则指向了这条服务端与客户端成功连接的”通路“
	if((clitfd = accept(servfd,(sockaddr *)& clit_addr,&clit_size)) == -1)
	{
		cout<<"accept failed : "<<strerror(errno)<<endl;
		return 0;
	}
 
	cout<<"Client access : "<<inet_ntoa(clit_addr.sin_addr)<<"  "<<ntohs(clit_addr.sin_port)<<endl;
 
 
	char buf[1024]; //用于读写数据
 
	while(1)
	{
		int rdstate;
		if((rdstate = read(clitfd,buf,sizeof(buf))) > 0 )//通过clitfd来读取数据，返回值为读取的长度
		{
			int i=0;
			cout<<"(Server)recv : ";
			for(i=0;i<rdstate;i++)
			{
				cout<<buf[i];
				buf[i] = toupper(buf[i]); //转换为大写
			}
			buf[i]='\0';
			cout<<endl;
			write(clitfd,buf,strlen(buf)); //发回客户端
		}
                else if(rdstate == 0)  //客户端退出
		{
			cout<<"client exit ! "<<endl;
			return 0;
		}
	}
 
	close(servfd);  //关闭套接字
	close(clitfd);  
	return 0;
}
