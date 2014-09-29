
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "bencode.h"
#include "helper.h"
#include "sha1.h"
#define MAXBUF 15360

struct bencode *tor;
struct bencode *announce;
struct bencode *info;
struct bencode *pieces;
int main(int argc, char *argv[])
{
	// Initializations
	char *a;
	char a_url[BUFSIZ];
	char a_domain[64];
    int a_port;
    char a_resource[128];
    size_t infoLen;
	char info_hash_hex[40];
	unsigned char info_hash[20];

	if(argc!=2)
	{
		printf("Usage: ./rubt file.torrent\n");
		exit(1);
	}

	// Load binary torrent (max file size 15KB) into buffer 
	unsigned char buffer[MAXBUF];

	int fsize;

	if(!(fsize=loadTorrent(buffer,argv[1])))
	{
		exit(1);
	}
	// Get necessary data fields
	tor 		= (struct bencode*) ben_decode(buffer,fsize);
	info 		= (struct bencode*) ben_dict_get_by_str((struct bencode*)tor,"info");
	pieces		= (struct bencode*) ben_dict_get_by_str((struct bencode*)info,"pieces");
	announce 	= (struct bencode*) ben_dict_get_by_str((struct bencode*)tor,"announce");
	a			= ben_print(announce);
	
	strcpy(a_url,a);
	free(a);

	// Strip quotes
	memmove (&a_url[0], &a_url[1], strlen (a_url));
	memmove (&a_url[strlen(a_url)-1], &a_url[strlen(a_url)], strlen (a_url)-(strlen(a_url)-1));

	// Parse announce url
    sscanf(a_url, "http://%63[^:]:%32d/%127[^\n]", a_domain, &a_port, a_resource);

	// Compute SHA1 info_hash
	unsigned char *bencode_info = ben_encode(&infoLen,info);
	sha1_compute(bencode_info,infoLen,info_hash);
	free(bencode_info);

	for(int i = 0; i<sizeof(info_hash); ++i)
  	{
    	sprintf(&info_hash_hex[i*2], "%02X", info_hash[i]);
 	}
	info_hash_hex[40]=0;

  	// Get url encoded info hash
	char info_hash_enc[BUFSIZ];
	encode(info_hash, info_hash_enc);

	printf("%s\n",info_hash_hex);
	printf("%s\n",info_hash_enc);
    // Generate peer id
    char peer_id[21];
    sprintf(peer_id, "-RUBT11-%012llu",  random12());
    printf("%s\n",peer_id);



	// Setup socket
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in a_serv; 
    struct hostent *hp;
    // Check if we have a valid ip already
    struct sockaddr_in tmp;
    a_serv.sin_family = AF_INET;
	a_serv.sin_port = htons(a_port);
    if(!inet_pton(AF_INET, a_domain, &tmp.sin_addr))
    {
    	hp=gethostbyname(a_domain);
    	if(hp==NULL)
	    {
	    	exit(1);
	    } 
	    memcpy(&a_serv.sin_addr, hp->h_addr_list[0], hp->h_length);

    }
    else
    {
    	inet_pton(AF_INET,a_domain, &a_serv.sin_addr);
	}
    int uploaded = 0;
    int downloaded =0;
    int listen_port = 6881;
    int left;
    printf("%i\n",strlen(ben_print(pieces)));

	
	if (connect(sockfd, (struct sockaddr *)&a_serv, sizeof(a_serv) )) 
	{
      exit(1);
  	}

  	char *USERAGENT = "Mozilla/5.0";
  	char *tpl = "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
  	// -5 is to consider the %s %s %s in tpl and the ending \0
  	char *query = (char *)malloc(strlen(a_domain)+strlen(a_resource)+strlen(USERAGENT)+strlen(tpl)-5);
  	sprintf(query, tpl, a_resource, a_domain, USERAGENT);
  	send(sockfd,query,strlen(query),0);
  	char recvbuf[BUFSIZ];
  	recv(sockfd,recvbuf,BUFSIZ,0);
  	printf("%s\n",recvbuf);
  	

	return 0;
}
