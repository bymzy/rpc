

#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <iostream>
#include <string>

#define debug_log(data)\
    {\
        std::stringstream ss;\
        ss<<data;\
        std::cout <<ss.str()<<std::endl;\
}\

#define error_log(data)\
    {\
        std::stringstream ss;\
        ss<<data;\
        std::cerr<<ss.str()<<std::endl;\
}\

#endif




