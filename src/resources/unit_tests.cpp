#include "stdafx.h"
#include "unit_tests.hpp"

#if defined(RELEASE_BUILD)
void UnitTests::runAllTests();
#else
#undef NDEBUG

#include "helper.hpp"
#include "logging.hpp"

namespace
{
    void testCompareSemVer()
    {
        assert(Util::CompareSemanticVersion("1.0.0", "1.0.0") == Util::VersionCompareResult::Equal);
        assert(Util::CompareSemanticVersion("1.0.0", "0.0.9") == Util::VersionCompareResult::Newer);
        assert(Util::CompareSemanticVersion("1.0.0", "1.0.1") == Util::VersionCompareResult::Older);
        assert(Util::CompareSemanticVersion("1.2a", "1.2.1") == Util::VersionCompareResult::Equal);
        assert(Util::CompareSemanticVersion("1.0.0", "1.0.0.0") == Util::VersionCompareResult::Equal);
        assert(Util::CompareSemanticVersion("1.0.0", "0.0.9.9") == Util::VersionCompareResult::Newer);
        assert(Util::CompareSemanticVersion("1.0.0", "1.0.1.1") == Util::VersionCompareResult::Older);
        assert(Util::CompareSemanticVersion("1.2a", "1.2.1.0") == Util::VersionCompareResult::Equal);
        assert(Util::CompareSemanticVersion("1.0.0", "1.0.0.0") == Util::VersionCompareResult::Equal);
        assert(Util::CompareSemanticVersion("1.0.0", "0.0.9.9") == Util::VersionCompareResult::Newer);
        assert(Util::CompareSemanticVersion("1.0.0", "1.0.1.1") == Util::VersionCompareResult::Older);
        assert(Util::CompareSemanticVersion("1.2a", "1.2.1.0") == Util::VersionCompareResult::Equal);
        spdlog::info("Unit Test: compareSemVersion completed successfully.");
    }






}




void UnitTests::runAllTests()
{
    testCompareSemVer();

}

#define NDEBUG  // NOLINT(clang-diagnostic-unused-macros)
#endif
