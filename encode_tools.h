#ifndef __ENCODE_TOOLS_H___
#define __ENCODE_TOOLS_H___

#include <map>
#include <string>
namespace encode_tools{
using std::map;
using std::string;

//base64编码函数
unsigned char *base64_encode(const unsigned char* str, int length); 

//base64解码函数
unsigned char *base64_decode(const unsigned char *str, int length,int ret_length);

//加码url
bool url_encode(const std::string &in, std::string &out);

//解码url
bool url_decode(const std::string &in, std::string &out);

//
int url_decode(char *str, int len);

//判断是否为十六进制数 
bool is_xdigit(char c);

//
int htoi(char *s);

//
int base64decode(char* src,int srclen,char* dst,int dstlen);

//
string add_padding(const string& b64_string);

//安全base64解码函数
string web_safe_base64decode(const string& encoded);

//安全base64编码函数
string web_safe_base64encode(const string& src_text);

//base64解码函数
string base64decode(const string& encoded);

//base64编码函数
string base64encode(const string& src_text);

}
#endif
