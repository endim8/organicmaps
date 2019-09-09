#include "testing/testing.hpp"

#include "search/postcode_points.hpp"
#include "search/query_params.hpp"
#include "search/search_tests_support/helpers.hpp"
#include "search/utils.hpp"

#include "generator/generator_tests_support/test_mwm_builder.hpp"

#include "storage/country_info_getter.hpp"

#include "indexer/search_string_utils.hpp"

#include "platform/platform.hpp"
#include "platform/platform_tests_support/scoped_file.hpp"

#include "coding/point_coding.hpp"

#include "base/file_name_utils.hpp"
#include "base/math.hpp"
#include "base/stl_helpers.hpp"
#include "base/string_utils.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

using namespace generator::tests_support;
using namespace platform::tests_support;
using namespace search::tests_support;
using namespace search;
using namespace std;

namespace
{
class PostcodePointsTest : public SearchTest
{
};

TokenSlice GetSlice(string const & query, QueryParams & params)
{
  search::Delimiters delims;

  vector<strings::UniString> tokens;
  SplitUniString(NormalizeAndSimplifyString(query), base::MakeBackInsertFunctor(tokens), delims);
  params.InitNoPrefix(tokens.begin(), tokens.end());

  TokenRange tokenRange(0, tokens.size());
  return TokenSlice(params, tokenRange);
}

UNIT_CLASS_TEST(PostcodePointsTest, Smoke)
{
  string const countryName = "Wonderland";

  Platform & platform = GetPlatform();
  auto const & writableDir = platform.WritableDir();
  string const testFile = "postcodes.csv";
  auto const postcodesRelativePath = base::JoinPath(writableDir, testFile);

  // <outward>,<inward>,<easting>,<northing>,<WGS84 lat>,<WGS84 long>,<2+6 NGR>,<grid>,<sources>
  ScopedFile const osmScopedFile(testFile,
                                 "aa11, 0, dummy, dummy, 0.0, 0.0, dummy, dummy, dummy\n"
                                 "aa11, 1, dummy, dummy, 0.1, 0.1, dummy, dummy, dummy\n"
                                 "aa11, 2, dummy, dummy, 0.2, 0.2, dummy, dummy, dummy\n");

  auto infoGetter = std::make_shared<storage::CountryInfoGetterForTesting>();
  infoGetter->AddCountry(
      storage::CountryDef(countryName, m2::RectD(m2::PointD(-1.0, -1.0), m2::PointD(1.0, 1.0))));

  auto const id = BuildCountry(countryName, [&](TestMwmBuilder & builder) {
    builder.SetPostcodesData(postcodesRelativePath, infoGetter);
  });

  auto handle = m_dataSource.GetMwmHandleById(id);
  auto value = handle.GetValue<MwmValue>();
  CHECK(value, ());
  TEST(value->m_cont.IsExist(POSTCODE_POINTS_FILE_TAG), ());

  PostcodePoints p(*value);
  {
    vector<m2::PointD> points;
    QueryParams params;
    p.Get(GetSlice("aa11 0", params), points);
    TEST_EQUAL(points.size(), 1, ());
    TEST(base::AlmostEqualAbs(points[0], m2::PointD(0.0, 0.0), kMwmPointAccuracy), ());
  }
  {
    vector<m2::PointD> points;
    QueryParams params;
    p.Get(GetSlice("aa11 1", params), points);
    TEST_EQUAL(points.size(), 1, ());
    TEST(base::AlmostEqualAbs(points[0], m2::PointD(0.1, 0.1), kMwmPointAccuracy), ());
  }
  {
    vector<m2::PointD> points;
    QueryParams params;
    p.Get(GetSlice("aa11 2", params), points);
    TEST_EQUAL(points.size(), 1, ());
    TEST(base::AlmostEqualAbs(points[0], m2::PointD(0.2, 0.2), kMwmPointAccuracy), ());
  }
  {
    vector<m2::PointD> points;
    QueryParams params;
    p.Get(GetSlice("aa11", params), points);
    TEST_EQUAL(points.size(), 3, ());
    sort(points.begin(), points.end());
    TEST(base::AlmostEqualAbs(points[0], m2::PointD(0.0, 0.0), kMwmPointAccuracy), ());
    TEST(base::AlmostEqualAbs(points[1], m2::PointD(0.1, 0.1), kMwmPointAccuracy), ());
    TEST(base::AlmostEqualAbs(points[2], m2::PointD(0.2, 0.2), kMwmPointAccuracy), ());
  }
}
}  // namespace
