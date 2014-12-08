#ifndef __xTOOLS_H___
#define __xTOOLS_H___

#include <map>
#include <string>
#include "fcgiapp.h"
#include <sys/time.h>

namespace xtools{

using std::map;
using std::string;

//获取请求中的环境变量(安全模式)
string get_envp(const char *name, FCGX_ParamArray &envp);

//获取请求中的参数生成参数字典
int get_params(string &query, map<string,string> &params);

//加码url
bool url_encode(const std::string &in, std::string &out);

//解码url
bool url_decode(const std::string &in, std::string &out);

//获取当前服务
char* get_time(char *timestr, int size, const char *format);

//将timeval结构的时间构式转换成字符串的格式，由format控制输出格式,
//size 一般要求大于26个字节
char* get_time_from_timeval(char *timestr,int size,const char *format,const struct timeval *from);

//获取当前时间，精确到微秒，timestr的空间必需 >= 26个字节长度
char* get_cur_time_2u(char *timestr);

//十六进制的字符串变成char类型值，即8位的整型, "0x1a" => 0x1a  "1a"=>0x1a
int hexstr2_int(const char *str,char *ret_data);

//十六进制的字符串变成int类型值, "0x123abcdef" => 0x123abcdef
int hexstr2_int(const char *str,int *ret);

//十六进制的字符串变成char类型的数组，"0x1a-0x2b-0x33" => arr[]={0x1a,0x2b,0x33},字符串中的“0x”也可以去掉
int str2array(const string &str,char *arr,int arr_size);

// 十六进制字符变成ascii
char hex_to_char(char c1, char c2); 
}
#endif
