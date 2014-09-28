
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "bencode.h"
#include "helper.h"
#include "sha1.h"
#define MAXBUF 15360

struct bencode *tor;
struct bencode *announce;
struct bencode *info;
int main(int argc, char *argv[])
{
	// Initializations
	char *a;
	char a_url[BUFSIZ];
	char domain[64];
    int port;
    char resource[128];
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
	announce 	= (struct bencode*) ben_dict_get_by_str((struct bencode*)tor,"announce");
	a			= ben_print(announce);
	
	strcpy(a_url,a);
	free(a);

	// Strip quotes
	memmove (&a_url[0], &a_url[1], strlen (a_url));
	memmove (&a_url[strlen(a_url)-1], &a_url[strlen(a_url)], strlen (a_url)-(strlen(a_url)-1));

	// Parse announce url
    sscanf(a_url, "http://%63[^:]:%32d/%127[^\n]", domain, &port, resource);

	printf("%s\n",a_url);
	
	printf("%s\n",ben_print(info));
	// Compute SHA1 info_hash
	unsigned char *bencode_info = ben_encode(&infoLen,info);
	sha1_compute(bencode_info,infoLen,info_hash);
	free(bencode_info);
	for(int i = 0; i<sizeof(info_hash); ++i)
  	{
    	sprintf(&info_hash_hex[i*2], "%02X", info_hash[i]);
 	}
  	info_hash_hex[40]=0;
	printf("%s\n",info_hash_hex);
	for(int i=0;i<sizeof(info_hash);++i)
	{
		printf("%c",info_hash[i]);
	}
    // Generate peer id
    char peer_id[21];
    sprintf(peer_id, "-RU%04d-%012llu", 0001, random12());
    printf("\n%s\n",peer_id);

    // Get url encoded info hash
	char info_hash_enc[sizeof(info_hash) * 3];
	encode(info_hash, info_hash_enc);
	printf("%s\n",info_hash_enc);
	// Send
	// Get peerlist
	int sockfd;
    struct sockaddr_in a_serv; 
    struct hostent *hp = gethostbyname(domain);   
	/*
	char sendline[BUFSIZ + 1], recvline[BUFSIZ + 1];
char* ptr;

size_t n;

/// Form request
snprintf(sendline, 200, 
     "GET %s HTTP/1.0\r\n"  // POST or GET, both tested and works. Both HTTP 1.0 HTTP 1.1 works, but sometimes 
     "Host: %s\r\n"     // but sometimes HTTP 1.0 works better in localhost type
     "Content-type: application/x-www-form-urlencoded\r\n"
     "Content-length: %d\r\n\r\n"
     "%s\r\n", info_hash_hex, a_url, (unsigned int)strlen(info_hash_enc), info_hash_enc);
printf("%s\n",sendline);
	
	  char *query;
  char *getpage = page;
  char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
  if(getpage[0] == '/'){
    getpage = getpage + 1;
    fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
  }
  // -5 is to consider the %s %s %s in tpl and the ending \0
  query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
  sprintf(query, tpl, getpage, host, USERAGENT);
  return query;
  */
	return 0;
}