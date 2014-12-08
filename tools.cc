#include <sstream>
#include "tools.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

namespace xtools{

#define FCGI_INSTALL 0

#if FCGI_INSTALL
string get_envp(const char *name, FCGX_ParamArray &envp) {
    const char* ptr = FCGX_GetParam(name, envp);
    if(ptr == NULL){
        return string("");
    }
    else {
        return string(ptr);
    }
}
#endif


int get_params(string &query, map<string,string> &params) {
    int num = 0;
    char lastflag = '\0';
    string key, val;
    query += "&";
    for (size_t len=query.length(), begin=0, split=0, i=0; i<len; ++i) {
        switch (query[i]) {
            case '=' :
                if (lastflag == '=') {
                    break;
                }
                lastflag = '=';
                key = query.substr(begin, i-begin);
                val = ""; 
                begin = i+1;
                split = i+1;
                break;
            case '&' :
                lastflag = '&';
                val = query.substr(split, i-split);
                //if (key.length()>0 && val.length()>0) {
                if (key.length()>0) {
                    params[key] = val;
                    num++;
                }   
                key = ""; 
                begin = i+1;
                split = i+1;
                break;
            default :
                break;
        }   
    }   
    return num;
}

bool is_xdigit(char c) {
    if ( (c>='A' && c<='F') || (c>='a' && c<='f') || (c>='0' && c<='9') ) {
        return true;
    }
    else {
        return false;
    }
}

char hex_to_char(char c1, char c2) {
    int n1, n2;
    if (c1>='0' && c1<='9')
        n1 = c1 - '0';
    else if (c1>='A' && c1<='F') 
        n1 = c1 - 'A' + 10;
    else if (c1>='a' && c1<='f') 
        n1 = c1 - 'a' + 10;
    if (c2>='0' && c2<='9')
        n2 = c2 - '0';
    else if (c2>='A' && c2<='F')
        n2 = c2 - 'A' + 10;
    else if (c2>='a' && c2<='f')
        n2 = c2 - 'a' + 10;
    char ch = (char)(n1*16+n2);
    return ch;
}

int hexstr2_int(const char *str,char *ret_data)
{
	if( str==NULL ||ret_data==NULL )
		return -1;
	int len;
	char *s=NULL;
	s=(char *)((str[0]=='0' && (str[1]=='X'||str[1]=='x') )?str+2:str);
	len=strlen(s);
	if(len> 2||len<= 0)
		return -1;
	int i,ret=0;
	char ch;
	for(i=0;i<len;i++)
	{
		ch=s[i];
		if( ch>='0'&&ch<='9' )
			ret = ret*16+(ch-'0');
		else if( ch>='a'&&ch<='f' )
			ret = ret*16+(ch-'a'+10);
		else if( ch>='A'&&ch<='F' )
			ret = ret*16+(ch-'A'+10);
		else
			return -1;
	}
	*ret_data = (char)ret;
	return 0;
}


int hexstr2_int(const char *str,int *ret)
{
	if( str==NULL ||ret==NULL)
		return -1;
	int i=0, rett=0, len,value=0;
	const char *s=NULL;
	s= (str[0]=='0' && (str[1]=='X'||str[1]=='x') )?str+2:str;
	len=strlen(s);
	if(len<=0)
		return -1;
	char *tmp=(char *)malloc(len+1);
	if(tmp==NULL)
		return -1;
	for(i=0;i<len;i++)
	{
		if( !( (s[i]>='a'&&s[i]<='f')||(s[i]>='A'&&s[i]<='F')||(s[i]>='0'&&s[i]<='9') ) )
			return -1;
		else if( isupper(s[i]))
			tmp[i]=tolower(s[i]);
		else
			tmp[i]=s[i];
	}
	for(i=0;i<len;i++)
	{
		value=(tmp[i]>='0'&&tmp[i]<='9')?tmp[i]-'0':tmp[i]-'a'+10;
		rett =rett*16+value;
	}
	*ret=rett;
	//printf("*ret=%d %p\n",*ret,*ret);
	free(tmp);	
	return 0;
}

int str2array(const string &str,char *arr,int arr_size)
{
	if(str.size()==0 )
		return -1;
	string tmp;
	int i=0;
	char ch;
	int begin=0;
	int end=0;
	for(;i<arr_size;i++)
	{
		if((end=str.find('-',begin))!= string::npos)
		{
			tmp=str.substr(begin,end-begin);
			if( 0!=hexstr2_int(tmp.c_str(),&ch) )
				return -1;	
			arr[i]=ch;
			begin=end+1;
		}
		else
		{
			tmp=str.substr(begin);
			if( 0!=hexstr2_int(tmp.c_str(),&ch) )
				return -1;	
			arr[i]=ch;
			break;
		}
	}
	return 0;	
}


bool url_decode(const std::string &in, std::string &out) {
    for(unsigned int i=0; i< static_cast<unsigned int>(in.length()); i++) {
        switch(in[i]) {
            case '+':
                out += ' ';
                break;
            case '%':
                if(isxdigit(in[i+1]) && isxdigit(in[i+2])) {
                    out += hex_to_char(in[i+1], in[i+2]);
                    i += 2;
                }
                else {
                    out += '%';
                }
                break;
            default:
                out += in[i];
                break;
        }
    }
    return true;
}

bool url_encode(const std::string &in, std::string &out) {
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i) {   
        if (in[i] == '%') {   
            if (i + 3 <= in.size()) {   
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {   
                    out += static_cast<char>(value);
                    i += 2;
                } else {
                    return false;
                }   
            } else {
                return false;
            }
        }
        else if (in[i] == '+') {
            out += ' ';
        } else {
            out += in[i];
        }
    }
    return true;
}

char* get_time(char *timestr, int size, const char *format) {
    if(format==NULL||timestr==NULL)
        return NULL;
    time_t timenow = time(NULL);
    struct tm *ptm = localtime(&timenow);
    strftime(timestr, size, format, ptm);
    return timestr;
}

char* get_time_from_timeval(char *timestr,int size,const char *format,const struct timeval *from)
{
    if(from==NULL||format==NULL||timestr==NULL)
        return NULL;
    time_t seconds = from->tv_sec;
    struct tm *ptm = localtime(&seconds);
    strftime(timestr,size,format,ptm);
    char usecs[6]={0};
    sprintf(usecs,"%ld",from->tv_usec);
    strcat(timestr,".");
    strcat(timestr,usecs);
    return timestr;
}

char* get_cur_time_2u(char *timestr)
{
	if( timestr == NULL )
		return NULL;
	struct timeval tval;
	gettimeofday(&tval,NULL);
	return get_time_from_timeval(timestr,26,"%Y-%m-%d %H:%M:%S",&tval);
}

}

