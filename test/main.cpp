#include <iostream>
#include<chrono>
//using namespace std;
using namespace std::chrono;

#ifdef _WIN32
#include <windows.h>   // Windows控制台API头文件
#define WIN32_LEAN_AND_MEAN  
#endif



int main()
{
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 设置控制台输出代码页为UTF-8
    #endif

    system("pause");
    return 0;
}
