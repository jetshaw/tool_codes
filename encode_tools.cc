#include <sstream>
#include "encode_tools.h"
#include<string.h>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include <iostream>
#include <openssl/bio.h>
#include <openssl/md5.h>
#include <openssl/pem.h>
#include <arpa/inet.h>
namespace encode_tools{
using namespace std;

typedef unsigned char uchar;

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

// Hex string to integer
// For example: 'ac' to 0xac
int htoi(char *s)
{
    int value;
    int c;

    c = ((uchar*)s)[0];
    if (isupper(c))
    {
        c = tolower(c);
    }
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

    c = ((uchar*)s)[1];
    if (isupper(c))
    {
        c = tolower(c);
    }
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

    return (value);
}

// Standard base64 decoder
int base64decode(char* src, int srclen, char* dst, int dstlen)
{
    BIO *bio, *b64 ;
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new_mem_buf(src, srclen);
    bio = BIO_push(b64, bio);
    int dstNewLen = BIO_read(bio, dst, dstlen);
    BIO_free_all(bio);
    return dstNewLen;
}


unsigned char *base64_encode(const unsigned char* str, int length) 
{
	static char base64_table[] = 
	{
		'A','B','C','D','E','F','G','H',
		'I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X',
		'Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n',
		'o','p','q','r','s','t','u','v',
		'w','x','y','z','0','1','2','3',
		'4','5','6','7','8','9','+','/',
		'\0'
	};
	static char base64_pad = '=';
	unsigned const char *current = (unsigned const char*)str;
	int i = 0;
	unsigned char* result=(unsigned char*)malloc(((length+3-length%3)*4/3+1)*sizeof(char));
	while(length>2)
	{/*keepgoinguntilwehavelessthan24bits*/
			result[i++]=base64_table[current[0]>>2];
			result[i++]=base64_table[((current[0]&0x03)<<4)+(current[1]>>4)];
			result[i++]=base64_table[((current[1]&0x0f)<<2)+(current[2]>>6)];
			result[i++]=base64_table[current[2]&0x3f];
			current+=3;
			length-=3;/*wejusthandle3octetsofdata*/
	}	
	if(length!=0)
	{
			result[i++]=base64_table[current[0]>>2];
			if(length>1){
					result[i++]=base64_table[((current[0]&0x03)<<4)+(current[1]>>4)];
					result[i++]=base64_table[(current[1]&0x0f)<<2];result[i++]=base64_pad;
			}
			else{
					result[i++]=base64_table[(current[0]&0x03)<<4];
					result[i++]=base64_pad;
					result[i++]=base64_pad;
			}
	}
	result[i]='\0';//printf("%s\n",result);
	return result;
}


unsigned char *base64_decode(const unsigned char *str, int length,int *ret_length) 
{ 
	static char base64_table[] = 
	{
		'A','B','C','D','E','F','G','H',
		'I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X',
		'Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n',
		'o','p','q','r','s','t','u','v',
		'w','x','y','z','0','1','2','3',
		'4','5','6','7','8','9','+','/',
		'\0'
	};
	static char base64_pad = '=';
	const unsigned char *current = str;
	int ch, i=0,j=0,k;
	static short reverse_table[256];
	static int table_built;
	unsigned char *result;
	if( ++table_built == 1 )
	{
		char *chp;
		for(ch=0;ch<256;ch++)
		{
			chp = strchr(base64_table,ch);
			if(chp)
			{
				reverse_table[ch] = chp-base64_table;
			}
			else
			{
				reverse_table[ch]=-1;
			}
		}
	}
	result = (unsigned char *)malloc(length+1);
	if( result == NULL )
		return NULL;
	while((ch=*current++)!='\0')
	{
		if(ch==base64_pad) break;
		if( ch==' ') ch='+';
		ch = reverse_table[ch];
		if( ch<0 ) continue;
		switch(i%4)
		{
			case 0:
				result[j]=ch<<2;
				break;
			case 1:
				result[j++] |= ch>>4;
				result[j]=(ch&0x0f)<<4;
				break;
			case 2:
				result[j++] |= ch>>2;
				result[j] = (ch&0x03)<<6;
				break;
			case 3:
				result[j++] |= ch;
				break;
		}
		i++;
		
	}
	k=j;
	if (ch == base64_pad)
	{
		switch(i%4)
		{
			case 0:
			case 1:
				free(result);
				return NULL;
			case 2:
				k++;
			case 3:
				result[k++] = 0 ;

		}
	}
	if(ret_length) 
	{
		*ret_length=j;
	}
	result[k]='\0';
	return result;
}



// Standard url decoder
int url_decode(char *str, int len)
{
    char *dest = str;
    char *data = str;

    while (len--) 
    { 
        if (*data == '+') 
        {
            *dest = ' ';
        }
        else if (*data == '%' && len >= 2 
                && isxdigit((int) *(data + 1))
                && isxdigit((int) *(data + 2))) 
        {
            *dest = (char) htoi(data + 1);
            data += 2;
            len -= 2;
        } 
        else 
        {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = '\0';
    return dest - str;
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

string add_padding(const string& b64_string)
{
    if( b64_string.size() %4==3 )
        return b64_string+"=";
    else if(b64_string.size()%4==2)
        return b64_string+"==";
    return b64_string;
}

string web_safe_base64decode(const string& encoded)
{
	if( encoded.size()==0 )
		return string("");
    string padded = add_padding(encoded);
    int32_t index = -1;
    while ((index = padded.find_first_of('-', index + 1)) != string::npos) {
        padded[index] = '+';
    }
    index = -1;
    while ((index = padded.find_first_of('_', index + 1)) != string::npos) {
        padded[index] = '/';
    }

    // base64 decode using openssl library.
    const int32_t kOutputBufferSize = 256;
    char output[kOutputBufferSize];

    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* bio = BIO_new_mem_buf(const_cast<char*>(padded.data()),
            padded.length());
    bio = BIO_push(b64, bio);
    int32_t out_length = BIO_read(bio, output, kOutputBufferSize);
    BIO_free_all(bio);
    return string(output, out_length);
}

string web_safe_base64encode(const string& src_text)
{
	if( src_text.size()==0 )
		return string("");
    string ret_str;
    BIO *bmem,*b64;
    BUF_MEM *bptr;
    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64,bmem);
    BIO_write(b64,src_text.data(),src_text.length());
    BIO_flush(b64);
    BIO_get_mem_ptr(b64,&bptr);
    char * buff = (char *)malloc(bptr->length);
	if( buff==NULL )
		return ret_str;
    memcpy(buff,bptr->data,bptr->length-1);
    buff[bptr->length-1]=0;
    BIO_free_all(b64);
    ret_str += buff;
	free(buff);
    return ret_str;
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

string base64encode(const string& src_text)
{
	if( src_text.size() == 0 )
		return string("");
	char *ret=NULL;
	ret = (char *)base64_encode((unsigned char *)src_text.c_str(),src_text.size());
	if( NULL == ret )
		return string("");
	string retstr(ret);
	free(ret);
	return retstr;
}

string base64decode(const string& encoded)
{
	if( encoded.size() == 0 )
		return string("");	
	char *ret=NULL;
	int ret_length;
	ret = (char *)base64_decode((unsigned char *)encoded.c_str(),encoded.size(),&ret_length);
	if( NULL == ret )
		return string("");
	string retstr(ret);
	free(ret);
	return retstr;
}

}
