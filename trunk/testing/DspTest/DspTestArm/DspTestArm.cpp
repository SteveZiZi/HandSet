// DspTestArm.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include "gtest/gtest.h"

int _tmain(int argc, _TCHAR* argv[])
{
    std::cout << "Running main() from gtest_main.cc\n";

    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    getchar();
}