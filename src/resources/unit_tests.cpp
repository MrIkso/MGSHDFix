#include "stdafx.h"
#include "unit_tests.hpp"

#if defined(RELEASE_BUILD)
void UnitTests::runAllTests()
{
    
}
#else
#undef NDEBUG

#include "logging.hpp"
#include "version_checking.hpp"

namespace
{
    void testCompareSemVer()
    {
        assert(VersionCheck::CompareSemanticVersion("1.0.0", "1.0.0") == VersionCheck::CompareResult::Equal);
        assert(VersionCheck::CompareSemanticVersion("1.0.0", "1.0.0.0") == VersionCheck::CompareResult::Equal);
        assert(VersionCheck::CompareSemanticVersion("1.0.0.0", "1.0") == VersionCheck::CompareResult::Equal);
        assert(VersionCheck::CompareSemanticVersion("01.02.03", "1.2.3") == VersionCheck::CompareResult::Equal);
        assert(VersionCheck::CompareSemanticVersion("", "0.0.0") == VersionCheck::CompareResult::Equal);

        assert(VersionCheck::CompareSemanticVersion("1.2a", "1.2.1") == VersionCheck::CompareResult::Equal);
        assert(VersionCheck::CompareSemanticVersion("1.2a", "1.2.1.0") == VersionCheck::CompareResult::Equal);
        assert(VersionCheck::CompareSemanticVersion("1.2b", "1.2.2.0") == VersionCheck::CompareResult::Equal);
        assert(VersionCheck::CompareSemanticVersion("1.2b", "1.2.2") == VersionCheck::CompareResult::Equal);

        assert(VersionCheck::CompareSemanticVersion("0.1.0", "0.0.9") == VersionCheck::CompareResult::Newer);
        assert(VersionCheck::CompareSemanticVersion("1.0.0", "0.0.9") == VersionCheck::CompareResult::Newer);
        assert(VersionCheck::CompareSemanticVersion("1.0.0", "0.0.9.9") == VersionCheck::CompareResult::Newer);
        assert(VersionCheck::CompareSemanticVersion("1.0.0.1", "1.0") == VersionCheck::CompareResult::Newer);
        assert(VersionCheck::CompareSemanticVersion("1.10.0", "1.9.999") == VersionCheck::CompareResult::Newer);
        assert(VersionCheck::CompareSemanticVersion("1.2b", "1.2a") == VersionCheck::CompareResult::Newer);
        assert(VersionCheck::CompareSemanticVersion("2.0.0", "1.999.999.999") == VersionCheck::CompareResult::Newer);
        assert(VersionCheck::CompareSemanticVersion("65535.65535.65535.65535", "65535.65535.65535.65534") == VersionCheck::CompareResult::Newer);

        assert(VersionCheck::CompareSemanticVersion("garbage", "1.0.0") == VersionCheck::CompareResult::Older);
        assert(VersionCheck::CompareSemanticVersion("1.0.0", "1.0.1") == VersionCheck::CompareResult::Older);
        assert(VersionCheck::CompareSemanticVersion("1.0.0", "1.0.1.1") == VersionCheck::CompareResult::Older);
        assert(VersionCheck::CompareSemanticVersion("1.1.0", "1.1a") == VersionCheck::CompareResult::Older);
        assert(VersionCheck::CompareSemanticVersion("1.2", "1.2.1") == VersionCheck::CompareResult::Older);
        assert(VersionCheck::CompareSemanticVersion("1.2a", "1.2.1.1") == VersionCheck::CompareResult::Older);
        assert(VersionCheck::CompareSemanticVersion("1.2a", "1.2b") == VersionCheck::CompareResult::Older);

        spdlog::info("Unit Test: compareSemVersion completed successfully.");
    }






}




void UnitTests::runAllTests()
{
    testCompareSemVer();

}

#define NDEBUG  // NOLINT(clang-diagnostic-unused-macros)
#endif
