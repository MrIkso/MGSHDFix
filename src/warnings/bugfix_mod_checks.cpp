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
        if (!Util::CRC32Check(sExePath / "us" / "demo" / "_bp" / "p070_01_p01.sdt", 0xAE678CBD)) //emma crash-fix from mgs2 better audio mod.
        {
        }




    }


}
