#include"xconfigor.h"
#include"xini_file.h"
#include"debug.h"
#include<string>
#include<iostream>
#include<vector>
#include<pthread.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>

#define TEST_XCONFIGOR 1

using namespace std;
void* configor_cb(void *arg);

xconfigor::xconfigor():is_running(true)
{ }

xconfigor::~xconfigor(){}

static bool is_update_needed(xrecord& record, unsigned int &cur_file_time)
{
    struct stat f_st;
    if(stat(record.file_name.c_str(),&f_st)!=0)
        return false;
    if(!S_ISREG(f_st.st_mode))
        return false;
    cur_file_time = f_st.st_mtime;
    if( cur_file_time <= record.last_updated_time)
        return false;
    return true;
}

int xconfigor::init(const string &file_name)
{
    if( true != regist(file_name,&config_data,xconfigor::reload,true))
    {
        return -1;
    }
    
    pthread_t tid;
    if( 0!= pthread_create(&tid,NULL,configor_cb,this))
    {
        ERROR("%s,%d it can not create thread[%s,%d]\n",__FUNCTION__,__LINE__,strerror(errno),errno);
        stop();
        return -1;
    }
    return 0;
}

bool xconfigor::reload(const string &file_name, void *context)
{
    xdouble_buffer<xconfigor_data> & confdata = *(xdouble_buffer<xconfigor_data> *)context;
    xconfigor_data & next_data = confdata.get_next();
    next_data.clear();
    xini_file ini_processor;
    if( true != ini_processor.init(file_name) )
    {
        ERROR("%s,%d  process file[%s] error!\n",__func__,__LINE__,file_name.c_str());
        return false;
    }
    next_data.check_interval = ini_processor.get("xconfigor","check-interval",10);
    INFO("check-interval= %d\n",next_data.check_interval); 
    next_data.retry_times = ini_processor.get("xconfigor","retry-times",10);
    INFO("retry-times = %d\n",next_data.retry_times);
    next_data.retry_interval = ini_processor.get("xconfigor","retry-interval",10);
    INFO("retry-interval = %d\n",next_data.retry_interval);
    confdata.alter();
    return true;
}

bool xconfigor::regist(const string &file_name, void *context, load_func reloader, bool run_imediately)
{
    if( true != locker.get_write_permission() ){
        return false;
    }
    vector<xrecord>::iterator iter = records.begin();
    for(;iter!=records.end();iter++)
    {
        if( file_name.compare(iter->file_name)==0 )
        {
            locker.release_permission();
            ERROR("%s,%d sorry,system does not support duplicated file name[%s] now,it will be ok later!\n",__FUNCTION__,__LINE__,file_name.c_str());
            return false;
        }
    }
    //check the file is ok?
    struct stat f_st={};
    if( 0!= stat(file_name.c_str(),&f_st))
    {
        ERROR("%s,%d can not get the stat of the file [%s]\n",__FUNCTION__,__LINE__,file_name.c_str());
        locker.release_permission();
        return false;
    }
    if( !S_ISREG(f_st.st_mode))
    {
        ERROR("%s,%d error: the file [%s] is not a reg file\n",__FUNCTION__,__LINE__,file_name.c_str());
        locker.release_permission();
        return false;
    }
    xrecord record;
    record.file_name=file_name;
    record.reloader=reloader;
    record.context = context;
    record.last_updated_time = f_st.st_mtime;
    records.push_back(record);
    if( run_imediately )
    {
        reloader(file_name,context);
    }
    locker.release_permission();
    return true;
}

void* configor_cb(void *arg)
{
    xconfigor &configor = *(xconfigor*)arg;
    if( configor.running()==false )
    {
        ERROR("is not running.................. \n");
        return NULL;
    }
    while( configor.running())
    {
        xconfigor_data& cdata = configor.config_data.get();
        if(true != configor.locker.get_write_permission())
        {
            sleep(cdata.check_interval);
            INFO("continue......................\n");
            continue;
        }
        
        vector<xrecord>::iterator iter = configor.records.begin();
        for(;iter!=configor.records.end();++iter)
        {
            INFO("for in running ......\n");
            unsigned int file_mtime = 0;
            if(!is_update_needed(*iter,file_mtime))
                continue;
            int retried_times = cdata.retry_times;
            do{
                bool update_result = iter->reloader(iter->file_name,iter->context);
                if(update_result)
                {
                    break;
                }
                usleep(cdata.retry_interval);
            }while((--retried_times)>0);
            if( retried_times <= 0 )
            {
                ERROR("can not be loaded successfully\n");
            }else
            {
                iter->last_updated_time = file_mtime;
                ERROR("has been loaded successfully\n");
            }
        }
        configor.locker.release_permission();
        sleep(cdata.check_interval);
    }
    return arg;
}

#if TEST_XCONFIGOR

class person
{
    public:
        string          name;
        unsigned int    old;
        int            sex;
        person()
        {
            name="unknow";
            old=0;
            sex=0;
        }
};

bool person_loader(const string &file_name, void *context)
{
    xdouble_buffer<person> &persons = *(xdouble_buffer<person>*)context;
    person &next = persons.get_next();
    xini_file file;
    INFO("do person_loader\n");
    if( true != file.init(file_name))
    {
        ERROR("xini_file.init error\n");
        return false;
    }
    next.name = file.get("person","name","error");
    next.old = file.get("person","old",0);
    next.sex = file.get("person","sex",0);
    persons.alter();
    return true;
}


class test_person
{
    public:
        int init(const string &file_name)
        {
           if( true != xconfigor::inst().regist(file_name,&data,person_loader,true))
           {
                ERROR("regist error\n");
                return -1; 
           }
            INFO("init ok\n");
            return 0;
        }
        void print()
        {
            std::cout<<"name:"<< data.get().name.c_str()<<"\n" <<std::endl;
            std::cout<<"old:"<<data.get().old<<"\n"<<std::endl;
            std::cout<<"sex:"<<data.get().sex<<"\n"<<std::endl;
        }
        xdouble_buffer<person> data;
};

int main(int argc, char** argv)
{
    if( argc<2 )
    {
        ERROR("no config file input\n");
        return -1;
    }
    string configfile(argv[1]);
    std::cout<<"configfile: "<<configfile.c_str()<<"\n"<<std::endl;
    xconfigor::inst().init(configfile);
    test_person per;
    string person_file("./conf/person.ini");
    if( 0!=per.init(person_file))
    {
        std::cout<<"per.init error\n"<<std::endl;
        return -1;
    }
    while(1)
    {
        sleep(1);
        per.print();
    }
    return 0;
}
#endif
