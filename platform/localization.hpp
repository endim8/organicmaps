#pragma once

#include <string>

#include "platform/measurement_utils.hpp"

namespace platform
{
struct LocalizedUnits
{
  std::string m_low;
  std::string m_high;
};

std::string GetLocalizedTypeName(std::string const & type);
std::string GetLocalizedBrandName(std::string const & brand);
std::string GetLocalizedString(std::string const & key);
std::string GetCurrencySymbol(std::string const & currencyCode);
extern std::string GetLocalizedMyPositionBookmarkName();

extern LocalizedUnits const & GetLocalizedDistanceUnits();
extern LocalizedUnits const & GetLocalizedAltitudeUnits();

extern std::string const & GetLocalizedSpeedUnits(measurement_utils::Units units);
extern std::string const & GetLocalizedSpeedUnits();
}  // namespace platform
