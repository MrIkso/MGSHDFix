#include "stdafx.h"
#include "bugfix_mod_checks.hpp"

#include "common.hpp"
#include "d3d11_api.hpp"
#include "logging.hpp"

namespace
{

}


void BugfixMods::Check()
{
    if (eGameType & MGS2)
    {
        if (!Util::SHA1Check(sExePath / "us" / "demo" / "_bp" / "p070_01_p01.sdt", "d80e4a625b50ab6d573a7c063ece5b038b97053b")) //emma crash-fix from mgs2 better audio mod.
        {
            //MessageBoxA(g_D3D11Hooks.MainHwnd, "fuck", "fuck", MB_ICONWARNING);
            //ExitProcess(1);
        }




    }


}
