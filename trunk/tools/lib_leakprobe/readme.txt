名称: 内存泄露检查器


用法: 把源文件加入工程编译
      通过调试输出或调试串口查看输出
      工程属性加上宏定义 USE_LKP_MEMLEAK
               如果需要探测Bitmap的泄露, 就要再加上USE_LKP_MEMLEAK_BITMAP

         
程序要正常退出才能看到内存泄露信息

主要看 cnt 11012, handle 0x1088960, size 22, type 1
这条信息, cnt代表第几次分配内存, handle代表内存地址, type代表分配的类型 是 malloc, 还是new 还是LocaleAlloc
一般泄露显示4条以内的都不认为是泄露, 因为那是mfc固有的泄漏

如果每次检测到 cnt 11012, handle 0x1088960   这两个值一样的, 就可以下断点
你看一下 lib_leakprobe.cpp 这个文件
void  Lkp_StartProbe ()
{
    Lkp_StartProbeMemLeak();
    //Lkp_BreakAtAllocCount(2649);
    //Lkp_BreakAtAllocHandle((void*)0x7105e0);

    Lkp_StartProbeMemUsage();
}