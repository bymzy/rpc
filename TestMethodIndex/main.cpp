

#include "Method.hpp"

int main()
{
#if 0
    Object *obj2 = new Object;
    obj2->Invoke2(2);
#endif

    PrintServiceImpl *ps = new PrintServiceImpl;
    ps->Invoke(0);
    ps->Invoke(1);

    PrintServiceStub *stub = new PrintServiceStub;
    stub->Print1();
    stub->Print2();

    return 0;
}







