#ifndef __CONFIG_H__
#define __CONFIG_H__


#include "xini_file.h"


class xconfig {

    public:

        static bool init(const std::string &file) { 
            return inst()._ini_file.init(file);
        }

        template <typename T>
        static T get(const std::string &section, const std::string &key, T value) {
            return Inst()._ini_file.get(section, key, value);
        } 

        static std::string get(const std::string &section, const std::string &key, const char *value) {
            return Inst()._ini_file.get(section, key, std::string(value));
        }

        //static const std::string &ErrorStr() { return Inst()._ini_file.ErrorStr(); }

    private:

        static xconfig &Inst() {
            static xconfig instance;
            return instance;
        }

        xconfig() {}
        ~xconfig() {}

    private:
        
        xini_file _ini_file;
};

#endif
