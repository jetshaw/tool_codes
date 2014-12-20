#ifndef __xCONFIGOR_H___
#define __xCONFIGOR_H___
#include<vector>
#include<string>
using namespace std;
#include"xrw_locker.h"
#include"xdouble_buffer.h"

//将文件内容载入到内存的载入函数
typedef bool (*load_func)(const string &file_name, void *context);

//数据结构描述符，用于描述动态加载配置文件的数据结构和相应的更新信息
class xrecord {
    public:
        xrecord() {}
        ~xrecord() {}

        string          file_name;//配置文件名称
        unsigned int    last_updated_time;//配置文件最后更新时间
        load_func       reloader;//将文件内容载入内存的载入函数
        void            *context;//存储内容的内存区，可以指向任何数据结构
};


//动态加载器的配置数据，用于描述动态加载器的加载间隔，次数，检查间隔
class xconfigor_data {
    public:
        xconfigor_data() : check_interval(10), retry_times(3),
                           retry_interval(100000) {
        }
        ~xconfigor_data(){}
        void clear() {
            check_interval = 10;
            retry_times = 3;
            retry_interval = 100000;
        }
    public:
        int             check_interval;//检查文件是否被更新的时间间隔，以秒为单位
        int             retry_times;//加载失败时的重试次数
        unsigned int    retry_interval;//加载失败时等待重试一次的间隔时间，以微秒为单位
};

//动态加载器
class xconfigor {
    public:
        ~xconfigor();
        static xconfigor& inst() {
            static xconfigor configor;
            return configor;
        }

        void stop() {
            is_running = false;
        }

        int init(const string &file_name);
        //动态加载器加载自身的配置文件的加载函数
        static  bool reload(const string &file_name, void *context);
        //注册函数，所有需要动态加载的数据都要调用
        bool regist(const string &file_name, void *context, load_func reloader, bool run_imediately);
        //动态加载线程的主函数,循环检查更新
        friend void* configor_cb(void *arg);

    private:
        xconfigor();
        bool running() {
            return is_running;
        }
    private:
        vector<xrecord>                     records;
        xdouble_buffer<xconfigor_data>      config_data;//动态加载器自身的配置数据
        bool                                is_running;
        xrw_locker                          locker;//读写锁
};


#endif //__xCONFIGOR_H___
