#include "net.h"

/**
 * multicast_add_membership - 加入组播组
 *
 * @sockfd: socket文件描述符
 * @multi_ip: 组播IP地址
 *
 * @RET: 成功返回0 失败返回-1 
 */
int multicast_add_membership(int sockfd, char *multi_ip)
{
	struct ip_mreq mreq;

	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = inet_addr(multi_ip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	return setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
}

/**
 * multicast_drop_membership - 退出组播组
 *
 * @sockfd: socket文件描述符
 * @multi_ip: 组播IP地址
 *
 * @RET: 成功返回0 失败返回-1 
 */
int multicast_drop_membership(int sockfd, char *multi_ip)
{
	struct ip_mreq mreq;

	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = inet_addr(multi_ip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	return setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
}

/**
 * local_udp_socket_bind - 绑定本地UDP套接字
 *
 * @port: UDP端口
 *
 * @RET: 成功返回0 失败返回-1
 */
int local_udp_socket_bind(int port)
{
	int sockfd;
	struct sockaddr_in si;
	u_int yes = 1;
	int ret;

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd < 0)
		return sockfd;

	memset(&si, 0, sizeof(struct sockaddr_in));
	si.sin_family = AF_INET;
	si.sin_addr.s_addr = htonl(INADDR_ANY);
	si.sin_port = htons(port);

	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	if (ret < 0)
		return ret;

	ret = bind(sockfd, (struct sockaddr *)(&si), sizeof(struct sockaddr));
	if (ret < 0)
		return ret;

	return sockfd;
}

/**
 * sockaddr_convert - 把ip地址和端口转换成网络地址
 *
 * @sin: 用来保存转换后的网络地址
 * @ip: 
 * @port:
 *
 * @RET: 成功返回0 失败返回-1
 */
int sockaddr_convert(struct sockaddr_in *sin, char *ip, int port)
{
	int ret;

	memset(sin, 0, sizeof(*sin));
	sin->sin_family = AF_INET;
	sin->sin_port = htons(port);

	ret = inet_pton(AF_INET, ip, &(sin->sin_addr));
	if (ret == 1)
		return 0;

	return -1;
}

#ifdef SOCKET_PIPE_SUPPORT
void make_pipe_name(char* buf,const char *name)
{
	sprintf(buf,"/tmp/%s",name);
}

/* portable named pipes */
int socket_server_pipe_create(const char *name)
{
	struct sockaddr_un sa;
	char pipename[64] = {0};
	make_pipe_name(pipename,name);
	int sock;
	sock=socket(AF_UNIX,SOCK_STREAM,0);
	sa.sun_family=AF_UNIX;
	strncpy(sa.sun_path,pipename,sizeof(sa.sun_path)-1);
	unlink(pipename);/*in case we didn't finished properly previous time */
	fchmod(sock,S_IRUSR|S_IWUSR);
	if (bind(sock,(struct sockaddr*)&sa,sizeof(sa))!=0){
		printf("Failed to bind command unix socket: %s \n",strerror(errno));
		return -1;
	}
	listen(sock,1);
	return sock;
}

int socket_server_pipe_accept_client(int server)
{
	struct sockaddr_un su;
	socklen_t ssize=sizeof(su);
	int client_sock=accept(server,(struct sockaddr*)&su,&ssize);
	return client_sock;
}


int socket_client_pipe_connect(const char *name)
{
	struct sockaddr_un sa;
	char pipename[64] = {0};
	make_pipe_name(pipename,name);
	int sock=socket(AF_UNIX,SOCK_STREAM,0);
	sa.sun_family=AF_UNIX;
	strncpy(sa.sun_path,pipename,sizeof(sa.sun_path)-1);
	if (connect(sock,(struct sockaddr*)&sa,sizeof(sa))!=0)
	{
		close(sock);
		return -1;
	}
	return sock;
}

int socket_client_pipe_read(int p, uint8_t *buf, int len)
{
	return read(p,buf,len);
}

int socket_client_pipe_write(int p, const uint8_t *buf, int len)
{
	return write(p,buf,len);
}

int socket_pipe_close(int spipe){
	return close(spipe);
}
#endif
