#pragma once

#include "KBDefines.h"


class IKBTestKeyReciver
{
public:
    virtual void  _OnTestKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags) =0;
    virtual void  _OnTestKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags) =0;
};


BOOL  InstallDbgHookKeyboard (IKBTestKeyReciver* keyReciver);
void  UninstallDbgHookKeyboard ();