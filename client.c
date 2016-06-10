// Yoad Shiran - 302978713 - ex2 http client

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

int write_socket(int socket, char *request);
char* delete_slash(char *line);
int check_port(char *port);
char *delete_port(char *line);
char *build_get_query(char *host, char *page);
int create_socket();
int hostname_to_ip(char * hostname , char* ip, int success);
int check_numbers(char *site);
void removeChar(char *str, char garbage);
int check_http(char *http, int len);
int check_pointes(char *site);
int connect_socket(int socket, struct sockaddr_in srv);

void free_all(char *site, char *ret, char *site_selash, char* date, char *day_char, char* hour_char, char* min_char, char *site_temp, char *adress_ip);
int main(int argc, char *argv[])
{
	int day=0, hour=0, min=0;
	char *day_char = NULL;
	char *hour_char = NULL;
	char *min_char = NULL;
	time_t now;char timebuf[128];
	now = time(NULL);
	int flag_header = 0;
	int flag_http = 0;
	int flag_time = 0;
	int flag_ip = 0;
	int flag_url = 0;
	char *adress_ip = NULL;
	int flag_ssl=0;
	char *site = NULL;
	char *ret = NULL;
	char *site_selash = NULL;
	char *site_temp = NULL;
	char *date = NULL;
	char ip[256];
	int i;
	int port=0;
	int socket;
	int success = 0;

	for (i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "-h"))
		{
			if (flag_header == 0)
			{
				flag_header = 1;
				continue;
			}
			else
			{
				free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
				perror ("worng input");
				exit(-1);
			}
		}
		else if (!strcmp(argv[i], "-d"))
		{
			if (flag_time == 0 && argv[i+1] != NULL)
			{
				if ((!strcmp(argv[i], "-d")) && (!strcmp(argv[i+1], "-d")))
				{
					free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
					perror("worng input");
					exit(-1);
				}
				else if (((!strcmp(argv[i], "-d")) && (!strcmp(argv[i+1], "-h"))))
				{
					free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
					perror("worng input");
					exit(-1);
				}
				flag_time = 1;
				date = (char*)calloc(1+strlen(argv[i+1]),sizeof(char));
				if (date == NULL)
				{
					perror ("calloc_date error");
					exit(-1);
				}
				strcpy(date, argv[i+1]);
				i++;
				continue;
			}
			else
			{
				perror ("worng input");
				free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
				exit(-1);
			}
		}
		if((check_http(argv[i],strlen(argv[i])) == 1) && flag_http==0)
		{
			perror("worng input");
			exit(-1);
		}
		else {flag_http = 1;}
		ret = argv[i];
		if (ret == NULL)
		{
			ret = strstr(argv[i], "https://");
		}
		if ((!strcmp(argv[i], "http://")) || (!strcmp(argv[i], "https://")))
		{
			perror ("worng input");
			if (flag_time == 1){free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);}
			exit(-1);
		}
		if (ret != NULL && flag_url == 0)
		{
			flag_url = 1;
			ret = argv[i];
			ret+=7;
			if (!strcmp(ret, "")){free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
				perror("worng input");
				exit(-1);}
			if (ret != NULL)
			{
				site = (char*)calloc(strlen(argv[i]),sizeof(char));
				if (site == NULL)
				{
					if (flag_time == 1){free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);}
				perror ("calloc site error");
				exit(-1);
				}
				strcpy(site,ret);	
				site_temp = (char*)calloc(strlen(argv[i]),sizeof(char));
				if (site_temp == NULL)
				{
				if (flag_time == 1){free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);}
				perror ("calloc site_temp error");
				exit(-1);
				}
				strcpy(site_temp,ret);
			}
			else if (flag_http == 1)
			{
				flag_ip = 1;
				adress_ip = (char*)calloc((1+strlen(argv[i])),sizeof(char));
				strcpy(adress_ip,argv[i]);
				site = (char*)calloc((1+strlen(argv[i])),sizeof(char));
				if (site == NULL)
				{
					if (flag_time == 1){free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);}
				perror ("calloc site error");
				exit(-1);
				}
				strcpy(site,adress_ip);	
				site_temp = (char*)calloc((1+strlen(argv[i])),sizeof(char));
				if (site_temp == NULL)
				{
					if (flag_time == 1){free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);}
				perror ("calloc site_temp error");
				exit(-1);
				}
				strcpy(site_temp,adress_ip);
			}
			else
			{
					perror ("worng input");
					if (flag_time == 1){free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);}
					exit(-1);
			}
		}
	}
	if (ret == NULL && flag_http == 0)
	{
		perror("worng input\n");
		free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
		exit(-1);
	}
	if (flag_ssl == 0){site+=7;}
	else {site+=8;}
	ret = strchr(site, '/');
		if (ret != NULL)
		{
			site_selash = (char*)calloc(strlen(site),sizeof(char));
			if (site_selash == NULL)
			{
				free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
				perror ("calloc site_selash error");
				exit(-1);
			}
			strcpy(site_selash,ret);
		}
		else
		{
			perror("worng input\n");
			site-=7;
			free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
			exit(-1);
		}
	if ((ret = strstr(site, ":")) != NULL)
	{
		i=0;
		if (check_port(ret) == 1)
		{
			perror("worng input\n");
			free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
			exit(-1);
		}
		memmove(&ret[i],&ret[i+1],strlen(ret) - i);
		if ((port = atoi(ret)) >= 65536 || (port = atoi(ret)) <=1)
		{
			perror("worng input\n");
			free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
			exit(-1);
		}
				delete_port(site_temp);
				delete_slash(site_temp);
				i++;
	}
	else
	{		
		delete_slash(site_temp);
		port = 80;
	}
	site-=7;
	ret = NULL;
	// convert the hostname to ip address
	// also check if the hostname is true
	struct hostent *hp;
	hostname_to_ip(site_temp , ip, success);
	if (success  == 1)
	{
		perror("worng input\n");
		exit(-1);
	}
	hp = gethostbyname(site_temp);
	if (hp == NULL)
	{
		herror("worng input(gethostbyname)");
		free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
		exit(-1);
	}
	int numofss =0;
	if (site_selash!=NULL)
	{
		numofss = strlen(site_selash);
	}
	char req[128 + strlen(site_temp) + numofss];
  	memset(req, '\0', sizeof(req));
  	if (flag_header == 1){strcat(req,"HEAD ");}
  	else{strcat(req,"GET ");}
  	strcat(req, "http://");
  	strcat(req, site_temp);
  	if (site_selash != NULL){strcat(req,site_selash);}
  	if (flag_time == 1)
  	{
  			memset(req, '\0', sizeof(req));
  			strcat(req,"HEAD ");
  			strcat(req, "http://");
  			strcat(req, site_temp);
  			if (site_selash != NULL)
  			{
  				strcat(req, site_selash);
  			}
  			strcat(req," HTTP/1.0");
			int j=0;
			int count =1;
			int k=0;
			int true_place = 0;
			{
				while((date[j] != ':'))
				{
					count++;
					j++;
					if (count == strlen(date))
					{
						free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
						perror("worng input\n");
						exit(-1);
					}
				}
				j=0;
				day_char = (char*)calloc(count,sizeof(char));
				if (day_char == NULL)
				{
				perror ("calloc day_char error");
				free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
				exit(-1);
				}
				count = 0;
				while(1)
				{
					if (date[j] >= 48 && date[j] <= 57)
					{
						day_char[k] = date[j];
						k++;
					}
					else if (date[j] == ':')
					{
						
						day = atoi(day_char);
						j++;
						true_place = j;
						count = 0;
						break;
					}
					else
					{
						free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
						perror("worng input\n");
						exit(-1);
					}
					j++;
					count++;
				}
				count = 0;
				k=0;
				while(date[j] != ':' && date[j] != '\0')
				{
					count++;
					j++;
					if (count == strlen(date))
					{
						free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
						perror("worng input\n");
						exit(-1);
					}
				}
				j = true_place;
				hour_char = (char*)calloc((1+count),sizeof(char));
				if (hour_char == NULL)
				{
					free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
					perror ("worng input");
					exit(-1);
				}
				while(1)
				{
					if (date[j] >= 48 && date[j] <= 57)
					{
						hour_char[k] = date[j];
						true_place++;
						k++;
					}
					else if (date[j] == ':')
					{
						hour = atoi(hour_char);
						j++;
						true_place = j;
						count = 0;
						k=0;
						break;
					}
					else
					{
						free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
						perror("worng input\n");
						exit(-1);
					}
					j++;
					count++;
				}
				while(date[j] != ':' && date[j] != '\0')
				{
					count++;
					j++;
					if (count == strlen(date))
					{
						free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
						perror("worng input\n");
						exit(-1);
					}
				}
				j = true_place;
				min_char = (char*)calloc((1+count),sizeof(char));
				if (min_char == NULL)
				{
					free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
					perror ("worng input");
					exit(-1);
				}
				while(1)
				{
					if (date[j] >= 48 && date[j] <= 57)
					{
						min_char[k] = date[j];
						k++;
					}
					else if (date[j] == ':' || date[j] == '\0')
					{
						min = atoi(min_char);
						count = 0;
						break;
					}
					else
					{
						free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
						perror("worng input\n");
						exit(-1);
					}
					j++;
					count++;
				}
			}
	if (flag_time == 11  )
	{
		perror("worng input\n");
		free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
		exit(-1);
	}
  	now=now-(day*24*3600+hour*3600+min*60); //where day, hour and min are the values
	//from the input
	strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));
	strcat(req, "\r\nIf-Modified-Since: ");
	strcat (req, timebuf);
	strcat (req, "\r\n\r\n");
  	}
  	else
  	{
  		strcat(req, " HTTP/1.0\r\n\r\n");
  	}
	socket = create_socket();
	struct sockaddr_in srv;
	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr))->s_addr;
	if(srv.sin_addr.s_addr == (in_addr_t)-1)
	{
		fprintf(stderr, "\nin_addr_t fail\n");
		exit(-1);
	}
	int sd;
	srv.sin_port = htons(port);
	socket = connect_socket(socket, srv);
  	printf("HTTP request =\n%s\nLEN = %d\n", req, strlen(req)); 
  	if(write(socket, req, sizeof(req))<0)
  	{
  		printf("Error writing from socket\n");
  		exit(0);	
  	}
  	char buffer[1024] = "";
  	memset(buffer, 0, sizeof(buffer));
  	int sd_read =0;
  	if ((sd = read(socket,buffer, sizeof(buffer))) < 0)
  	{
  		herror("Error reading from socket\n");
  		exit(-1);
  	}
  	sd_read = sd;
  	while (sd > 0)
  	{
  		printf("%s",buffer);
  		memset(buffer, 0, sizeof(buffer));
  		if ((sd = read(socket,buffer, sizeof(buffer))) < 0)
	  	{
	  		herror("Error reading from socket\n");
	  		exit(-1);
	  	}
	  	sd_read+=sd;
  	}
	
	if (buffer!=NULL)
	{
		printf("%s\n",buffer);
	}
	close(socket);
	printf("\n Total received response bytes: %d\n",sd_read);
	if (flag_ip == 1)
	{
		if (flag_ssl == 0){adress_ip-=7;}
		else{adress_ip-=8;}
	}
  	free_all(site,ret,site_selash,date,day_char,hour_char,min_char,site_temp,adress_ip);
  	return 0;
}
int hostname_to_ip(char * hostname , char* ip, int success)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;    
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        success = 1;
        return success;
    }
    addr_list = (struct in_addr **) he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
     success = 1;
    return success;
}
int create_socket()
{
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}
	return fd;
}
int connect_socket(int socket, struct sockaddr_in srv)
{
	if(connect(socket, (struct sockaddr*)&srv, sizeof(srv)) < 0)
	{
    perror("Could not connect");
    exit(-1);
  	}
  	return socket;
}
char *delete_port(char *line)
{
    int i=0,j;
    while(line[i] != '\0')
    {
    	if ( line[i]== ':'){break;}
    	i++;
    }
    while(line[i]!='\0')
    {
        while (!(((line[i]>='a') && (line[i]<='z')) || (((line[i]>='A') && (line[i]<='Z')) || (line[i]=='\0'))))
        {
        	if (line[i]== '.' || line[i] == '/'){break;}
            for(j=i;line[j]!='\0';++j)
            {
                line[j]=line[j+1];
            }
            line[j]='\0';
        }
        i++;
    }
    return line;
}
int check_pointes(char *site)
{
	int i;
	int counter = 0;
	int flag =0;
	for (i = 0; site[i] != '\0'; ++i)
	{
		if (site[i] == '/'){break;}
		if (site[i] == '.')
		{
			counter++;
			if (flag == 1)
			{
				herror("Problem with The URL Adress !\n");
				return 1;
			}
		}
		if (site[i] == ':')	{flag = 1;}
	}
	if (counter >=1){return 0;}
	return 1;
}
int check_port(char *port)
{
	int i;
	int counter1=0;
	for (i = 0; port[i] != '\0'; ++i)
	{
		if(port[i] == 58)
		{
			counter1++;
			if (counter1>1)	{return 1;}
			continue;
		}
		else if (port[i] == 47){break;}
		else if (port[i] < 48) // ASCII 48 = 0
		{
			// printf("DEBUG[1] number in ascii is %d\n", port[i]);
			return 1;
		}
		else if(port[i] > 57) // ASCII 57 = 9
		{
			// printf("DEBUG[2] number in ascii is %d\n", port[i]);
			return 1;
		}
	}
	if (port[i-1] == ':')
	{
		return 1;
	}
	return 0;	
}
char* delete_slash(char *line)
{
	int i=0;
    while(line[i] != '\0')
    {
    	if ( line[i]== '/'){break;}
    	i++;
    }
    while(line[i]!='\0')
    {
        line[i]=line[i+1];
        line[i]='\0';
        i++;
    }
    return line;
}
// void removeChar(char *str, char garbage) {
// 	printf("%s\n", str);
// 	int i=0;
// 	int max =6;
// 	for (i = 0; i < max; ++i)
// 	{
// 		if (str[i] == 'h' && str[i+1] == 't' && str[i+3] == 'p' && str[i+4] == ':' && str[i+5] == '/')
// 		{
// 			memset(str, '\0', sizeof(str));
// 		}
// 	}
// }
int check_http(char *http, int len)
{
	int counter=0;
	if (len < 7){return 1;}
	if (http[0] == 72 || http[0] == 104){counter++;}
	if (http[1] == 84 || http[1] == 116){counter++;}
	if (http[2] == 84 || http[2] == 116){counter++;}
	if (http[3] == 80 || http[3] == 112){counter++;}
	if (http[4] == 58){counter++;}
	if (http[5] == 47 && http[6] == 47){counter++;}
	if (counter != 6){return 1;}
	return 0;
}
void free_all(char *site, char *ret, char *site_selash, char* date, char *day_char, char* hour_char, char* min_char, char *site_temp, char *adress_ip)
{
	if (day_char != NULL){free(day_char);}
	if (hour_char != NULL){free(hour_char);}
	if (min_char != NULL){free(min_char);}
	if (site_selash != NULL){free(site_selash);}
	if (site_temp != NULL){free(site_temp);}
	if (site != NULL){free(site);}
	if (date != NULL){free(date);}
	ret = NULL;
	if (adress_ip != NULL){free(adress_ip);}
}
