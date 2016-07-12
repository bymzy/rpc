

#include <unistd.h>
#include <fcntl.h>

#include <string>
#include <iostream>

#include "Msg.hpp"

void TestIntAndString()
{
    Msg msg;
    int a =13, b;
    std::string s = "abcdefg", d;
    msg << s;
    msg << a;
    msg >> d;
    msg >> b;
    std::cout << s << ", " << a << std::endl;
    std::cout << d << ", " << b << std::endl;
}

int main(int argc, char *argv[])
{
    TestIntAndString();

    int fd1 = -1;
    int fd2 = -1;

    do {
        fd1 = open(argv[1], O_RDONLY);
        if (fd1 < 0) {
            std::cerr << "open file failed " << argv[1] << std::endl;
            break;
        }

        fd2 = open(argv[2], O_WRONLY | O_CREAT, 0644);
        if (fd2 < 0) {
            std::cerr << "open file failed " << argv[2] << std::endl;
        }
        
        int ret = 0;
        while(1) {
            ret = 0;
            Msg msg;
            char buf[1024 * 1024 * 1];
            ret = read(fd1, buf, 1024 * 1024);
            if (ret > 0) {
                msg.WriteByte(buf, ret);
            } else {
                std::cout << "read complete!" << std::endl;
                break;
            }

            char buf2[1024 * 1024 * 1];
            msg.ReadByte(buf2, ret);
            write(fd2, buf2, ret);
        } 
    } while(0);
    
    if (fd1 != -1) {
        close(fd1);
    }

    if (fd2 != -1) {
        close(fd2);
    }

    return 0;
}



