#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#define MAXBUF 15360

int loadTorrent(unsigned char *buf, char *fname)
{
	FILE *fp = fopen(fname, "rb");
	if (!fp)
  	{
  		return 0;
  	}
    fseek(fp, 0, SEEK_END);
    int fSize = ftell(fp);
    rewind(fp);
    fread(buf, 1, fSize, fp);
    // Check for max size
    if(fSize>=MAXBUF)
    {
      exit(1);
    }
    fclose(fp);
    return fSize;
}
unsigned long long random12()
{
     unsigned long long n = 0;
     do
     {
        srand(time(NULL));
         n *= RAND_MAX;
         n += rand();
     } while(n < 100000000000);
     return n % 1000000000000;
}
void encode(unsigned char *s, char *enc)
{
  char rfc3986[256];
  //char html5[256];
  for (int i = 0; i < 256; i++) 
  {
    rfc3986[i] = isalnum(i)||i == '~'||i == '-'||i == '.'||i == '_'
      ? i : 0;
    //html5[i] = isalnum(i)||i == '*'||i == '-'||i == '.'||i == '_'
    //  ? i : (i == ' ') ? '+' : 0;
  }
  for (; *s; s++) 
  {
    if (rfc3986[*s]) sprintf(enc, "%c", rfc3986[*s]);
    else        sprintf(enc, "%%%02X", *s);
    while (*++enc);
  }
}
