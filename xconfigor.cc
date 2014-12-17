#include"xconfigor.h"

xconfigor::xconfigor:is_running(true)()
{ }

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
    xdouble_buffer& confdata = *(xdouble_buffer<xconfigor_data> *)context;
    xconfigor_data& next_data = confdata.get_next();
    next_data.clear();
    xini_file ini_processor;
    if( true != ini_processor.init(file_name) )
    {
        ERROR("%s,%d  process file[%s] error!\n",__func__,__LINE__,file_name.c_str());
        return false;
    }
    next_data.check_interval = ini_processor.get("xconfigor","check-interval",10);
    INFO("check-interval= %d\n",next_data.check_interval); 
    next_data.retry_times = ini_processor.get("xconfigor","retry-times",1000);
    INFO("retry-times = %d\n",next_data.retry_times);
    next_data.retry_interval = ini_processor.get();
    INFO("retry_interval = %d\n",next_data.retry_interval);
    confdata.alter();
    return true;
}

bool xconfigor::regist(const string &file_name, void *context, load_func reloader, bool run_imediately)
{
    if( true != locker.get_write_permisson() ){
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

}

int main(int argc, char** argv)
{
    if( argc<2 )
    {
        ERROR("no config file input\n");
        return -1;
    }
    string configfile(argv[1]);
    xconfigor.inst().init(configfile);
    while(1)
    {
        sleep(1);
    }
    return 0;
}
