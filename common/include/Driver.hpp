

#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__

class Driver {
public:
    Driver()
    {}
    virtual ~Driver(){}

public:
    void RegistReadEvent(int fd);
    void RegistWriteEvent(int fd);
};


#endif


