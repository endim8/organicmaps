#include "platform/localization.hpp"
#include "platform/get_text_by_id.hpp"
#include "platform/measurement_utils.hpp"
#include "platform/settings.hpp"

#include <ctime>
#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>

namespace platform
{
namespace
{
enum class MeasurementType
{
  Distance,
  Speed,
  Altitude
};

LocalizedUnits const & GetLocalizedUnits(measurement_utils::Units units, MeasurementType measurementType)
{
  static LocalizedUnits const lengthImperial = {GetLocalizedString("ft"), GetLocalizedString("mi")};
  static LocalizedUnits const lengthMetric = {GetLocalizedString("m"), GetLocalizedString("km")};

  static LocalizedUnits const speedImperial = {GetLocalizedString("ft"), GetLocalizedString("miles_per_hour")};
  static LocalizedUnits const speedMetric = {GetLocalizedString("m"), GetLocalizedString("kilometers_per_hour")};

  switch (measurementType)
  {
  case MeasurementType::Distance:
  case MeasurementType::Altitude:
    switch (units)
    {
    case measurement_utils::Units::Imperial: return lengthImperial;
    case measurement_utils::Units::Metric: return lengthMetric;
    }
    break;
  case MeasurementType::Speed:
    switch (units)
    {
    case measurement_utils::Units::Imperial: return speedImperial;
    case measurement_utils::Units::Metric: return speedMetric;
    }
  }
  UNREACHABLE();
}
}  // namespace

std::string GetLocalizedStringWithLocale(std::string locale, std::string key)
{
  //where to keep platform-independent strings, and how to load/query?
  std::string buildPath = filesystem::exists("/res/values/") ? "True" : "False";
  return buildPath;
}

std::string GetLocalizedString(std::string const & key)
{
  auto locale = platform::GetCurrentLocale();
  return GetLocalizedStringWithLocale(locale.m_language, key);
}



LocalizedUnits const & GetLocalizedDistanceUnits()
{
  return GetLocalizedUnits(measurement_utils::GetMeasurementUnits(), MeasurementType::Distance);
}

LocalizedUnits const & GetLocalizedAltitudeUnits()
{
  return GetLocalizedUnits(measurement_utils::GetMeasurementUnits(), MeasurementType::Altitude);
}

const std::string & GetLocalizedSpeedUnits(measurement_utils::Units units)
{
  return GetLocalizedUnits(units, MeasurementType::Speed).m_high;
}

std::string const & GetLocalizedSpeedUnits()
{
  return GetLocalizedSpeedUnits(measurement_utils::GetMeasurementUnits());
}

//old stuff
std::string GetLocalizedTypeName(std::string const & poiType)
{
  return GetLocalizedString("hello");
  //return poiType;
}

std::string GetLocalizedBrandName(std::string const & brand){return brand;}
std::string GetCurrencySymbol(std::string const & currencyCode){return currencyCode;}

std::string GetLocalizedMyPositionBookmarkName()
{
  std::time_t t = std::time(nullptr);
  char buf[100] = {0};
  (void)std::strftime(buf, sizeof(buf), "%Ec", std::localtime(&t));
  return buf;
}

}  // namespace platform
