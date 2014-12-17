#ifndef __xCONFIGOR_H___
#define __xCONFIGOR_H___
#include<vector>
#include<string>
using namespace std;
#include"xrw_locker.h"
#include"xdouble_buffer.h"

typedef bool (*load_func)(const string &file_name, void *context);

class xrecord {
    public:
        xrecord() {}
        ~xrecord() {}

        string          file_name;
        unsigned int    last_updated_time;
        load_func       reloader;
        void            *context;
};

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
        int             check_interval;
        int             retry_times;
        unsigned int    retry_interval;
};


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
        static  bool reload(const string &file_name, void *context);
        bool regist(const string &file_name, void *context, load_func reloader, bool run_imediately);
        friend void* configor_cb(void *arg);

    private:
        xconfigor();
        bool running() {
            return is_running;
        }
    private:
        vector<xrecord>                     records;
        xdouble_buffer<xconfigor_data>      config_data;
        bool                                is_running;
        xrw_locker                          locker;
};


#endif //__xCONFIGOR_H___
