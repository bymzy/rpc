

#include <iostream>
#include <stdio.h>
#include <map>

template<class Type>
class Service 
{
public:
    typedef void (Type::* Method)();

public:
    Service():mMethodCount(0)
    {
    }
    ~Service()
    {
        delete[] mMethodList;
    }


public:
    virtual void Init() = 0;
    void SetMethodCount(int count)
    {
        mMethodCount = count;
        mMethodList = new Method[mMethodCount];
    }

    int AddMethod(int index, Method method)
    {
        if (index < 0 || index >= mMethodCount) {
            return 1;
        }

        mMethodList[index] = method;
        return 0;
    }

    Service::Method GetMethod(int index)
    {
        return mMethodList[index];
    }

    int GetMethodIndex(Service::Method method)
    {
        for (int i = 0;i < mMethodCount;++i) {
            if (method == mMethodList[i]) {
                return i;
            }
        }

        return -1;
    }


public:
    int mMethodCount;
    Method* mMethodList;
};

class PrintService : public Service<PrintService>
{
public:
    PrintService():Service<PrintService>()
    {
        Init();
    }

public:
    virtual void Init()
    {
        SetMethodCount(2);

        Service<PrintService>::Method m1 = &PrintService::Print1;
        AddMethod(0, m1);

        Service<PrintService>::Method m2 = &PrintService::Print2;
        AddMethod(1, m2);
    }

    void Invoke(int index)
    {
        (this->*(GetMethod(index)))();
    }

public:
    virtual void Print1()
    {
        std::cout << "not implement" << std::endl;
    }

    virtual void Print2()
    {
        std::cout << "not implement" << std::endl;
    }
};

class PrintServiceImpl : public PrintService
{
public:
    virtual void Print1()
    {
        std::cout << "print 1" << std::endl;
    }

    virtual void Print2()
    {
        std::cout << "print 2" << std::endl;
    }
};

class PrintServiceStub : public PrintService
{
public:
    virtual void Print1()
    {
        int index = GetMethodIndex(&PrintService::Print1); 
        std::cout << "call method with index " << index << std::endl;
    }

    virtual void Print2()
    {
        int index = GetMethodIndex(&PrintService::Print2); 
        std::cout << "call method with index " << index << std::endl;
    }
};



#if 0
class Object
{
public:
    typedef void (Object::* Method)();

    Object()
    {
        Method p1 = &Object::Print1;
        Method p2 = &Object::Print2;
        mMethodBoard.insert(std::make_pair(1, p1));
        mMethodBoard.insert(std::make_pair(2, p2));
    }

public:
    static void Invoke(Object * obj, int index)
    {
        std::map<int, Method>::iterator iter = obj->mMethodBoard.end();
        iter = obj->mMethodBoard.find(index);
        if (obj->mMethodBoard.end() == iter) {
            std::cerr << "not implement!" << std::endl;
        }
        Method method = iter->second;
        (obj->*method)();
    }

    void DebugMethod()
    {
        std::map<int, Method>::iterator iter = mMethodBoard.begin();
        
        for (; iter != mMethodBoard.end(); ++iter) {
            std::cout << std::hex << (iter->second) << std::endl;
        }
    }

    void Invoke2(int index)
    {
        std::map<int, Method>::iterator iter = mMethodBoard.end();
        iter = mMethodBoard.find(index);
        if (mMethodBoard.end() == iter) {
            std::cerr << "not implement!" << std::endl;
        }
        Method method = iter->second;
        (this->*method)();
    }

public:
    void Print1()
    {
        std::cout << "print 1" << std::endl;
    }

    void Print2()
    {
        std::cout << "print 2" << std::endl;
    }

public:
    std::map<int, Method> mMethodBoard;
};
#endif


