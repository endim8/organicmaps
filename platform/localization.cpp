#include "platform/localization.hpp"

#include "platform/settings.hpp"
#include "platform/units.hpp"

#include <string>
#include "map"

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

std::map<Units, std::string> length_units = {
        {Units::Yard, "yd"},
        {Units::Foot, "ft"},
        {Units::Mile, "mi"},
        {Units::Kilometer, "km"},
        {Units::Meter, "m"}
};

std::map<Units, std::string> speed_units = {
        {Units::Yard, "yd"},
        {Units::Foot, "ft"},
        {Units::Mile, "miles_per_hour"},
        {Units::Kilometer, "kilometers_per_hour"},
        {Units::Meter, "m"}
};

LocalizedUnits const & GetLocalizedUnits(std::vector<Units> units, MeasurementType measurementType)
{
  switch (measurementType)
  {
  case MeasurementType::Distance:
  case MeasurementType::Altitude:
    return {GetLocalizedString(length_units(units[0])), GetLocalizedString(length_units(units[1]))};
    break;
  case MeasurementType::Speed:
    return {GetLocalizedString(speed_units(units[0])), GetLocalizedString(speed_units(units[1]))};
  }
  UNREACHABLE();
}
}  // namespace

LocalizedUnits const & GetLocalizedDistanceUnits()
{
  return GetLocalizedUnits(measurement_utils::GetMeasurementUnits(), MeasurementType::Distance);
}

LocalizedUnits const & GetLocalizedAltitudeUnits()
{
  return GetLocalizedUnits(measurement_utils::GetMeasurementUnits(), MeasurementType::Altitude);
}

const std::string & GetLocalizedSpeedUnits(Units units)
{
  return GetLocalizedUnits(units, MeasurementType::Speed).m_high;
}

std::string const & GetLocalizedSpeedUnits()
{
  return GetLocalizedSpeedUnits(measurement_utils::GetMeasurementUnits());
}
}  // namespace platform
