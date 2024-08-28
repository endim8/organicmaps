#include "distance.hpp"

#include "platform/locale.hpp"
#include "platform/localization.hpp"
#include "platform/measurement_utils.hpp"
#include "platform/units.hpp"

#include "base/assert.hpp"

namespace platform
{
using namespace measurement_utils;

namespace
{
Distance MetersTo(double distance, Units units)
{
  switch (units)
  {
  case Units::Meter:
    return Distance(distance);
  case Units::Kilometer:
    return {distance / 1000, Units::Kilometer};
  case Units::Foot:
    return {MetersToFeet(distance), Units::Foot};
  case Units::Mile:
    return {MetersToMiles(distance), Units::Mile};
  case Units::Yard:
    return {MetersToYards(distance), Units::Yard};
  default: UNREACHABLE();
  }
}

Distance KilometersTo(double distance, Units units)
{
  return MetersTo(distance * 1000, units);
}

Distance FeetTo(double distance, Units units)
{
  switch (units)
  {
  case Units::Meter:
    return {FeetToMeters(distance), Units::Meter};
  case Units::Kilometer:
    return {FeetToMeters(distance) / 1000, Units::Kilometer};
  case Units::Mile:
    return {FeetToMiles(distance), Units::Mile};
  case Units::Yard:
    return {FeetToYards(distance), Units::Yard};
  default: UNREACHABLE();
  }
}

Distance MilesTo(double distance, Units units)
{
  switch (units)
  {
  case Units::Meter:
    return {MilesToMeters(distance), Units::Meter};
  case Units::Kilometer:
    return {MilesToMeters(distance) / 1000, Units::Kilometer};
  case Units::Foot:
    return {MilesToFeet(distance), Units::Foot};
  default: UNREACHABLE();
  }
}

Distance YardsTo(double distance, Units units)
{
  switch (units)
  {
    case Units::Meter:
      return {YardsToMeters(distance), Units::Meter};
    case Units::Kilometer:
      return {YardsToMeters(distance) / 1000, Units::Kilometer};
    case Units::Foot:
      return {YardsToFeet(distance), Units::Foot};
    default: UNREACHABLE();
  }
}

double WithPrecision(double value, uint8_t precision)
{
  if (precision == 0)
    return std::round(value);

  double const factor = base::PowUint(10.0, precision);
  return std::round(value * factor) / factor;
}
}  // namespace

Distance::Distance() : Distance(-1.0) {}

Distance::Distance(double distanceInMeters) : Distance(distanceInMeters, Units::Meter) {}

Distance::Distance(double distance, Units units) : m_distance(distance), m_units(units) {}

Distance Distance::CreateFormatted(double distanceInMeters)
{
  return Distance(distanceInMeters).ToPlatformUnitsFormatted();
}

std::string Distance::FormatAltitude(double meters)
{
  Distance elevation = Distance(fabs(meters)).To(GetMeasurementUnits()[1]);

  ASSERT(elevation.IsLowUnits(), ());
  elevation.m_distance = WithPrecision(elevation.m_distance, 0);

  auto res = elevation.ToString();
  return meters < 0 ? "-" + res : res;
}

bool Distance::IsValid() const { return m_distance >= 0.0; }

bool Distance::IsLowUnits() const { return m_units == Units::Meter || m_units == Units::Foot; }

bool Distance::IsHighUnits() const { return !IsLowUnits(); }

Distance Distance::To(Units units) const
{
  if (m_units == units)
    return *this;

  /// @todo These double switches can be replaced with 4x4 factors matrix.
  switch (m_units)
  {
  case Units::Meter: return MetersTo(m_distance, units);
  case Units::Kilometer: return KilometersTo(m_distance, units);
  case Units::Foot: return FeetTo(m_distance, units);
  case Units::Mile: return MilesTo(m_distance, units);
  case Units::Yard: return YardsTo(m_distance, units);
  default: UNREACHABLE();
  }
}

Distance Distance::ToPlatformUnitsFormatted() const
{
  return To(GetMeasurementUnits()[1]).GetFormattedDistance();
}

double Distance::GetDistance() const
{
  return m_distance;
}

Units Distance::GetUnits() const { return m_units; }

std::string Distance::GetDistanceString() const
{
  if (!IsValid())
    return "";

  // Default precision is 0 (no decimals).
  int precision = 0;

  // Set 1 decimal precision for high distances (km, miles) lower than 10.0 (9.5, 7.0,...).
  if (m_distance < 10.0 && IsHighUnits())
    precision = 1;

  return ToStringPrecision(m_distance, precision);
}

std::string Distance::GetUnitsString() const
{
  switch (m_units)
  {
  case Units::Meter: return GetLocalizedString("m");
  case Units::Kilometer: return GetLocalizedString("km");
  case Units::Foot: return GetLocalizedString("ft");
  case Units::Mile: return GetLocalizedString("mi");
  default: UNREACHABLE();
  }
}

Distance Distance::GetFormattedDistance() const
{
  ASSERT(IsValid(), ());

  // To low units.
  Distance res;
  if (m_units == Units::Kilometer)
    res = To(Units::Meter);
  else if (m_units == Units::Mile)
    res = To(Units::Foot);
  else
    res = *this;

  double lowRound = std::round(res.m_distance);
  // Round distances over 100 units to 10 units, e.g. 112 -> 110, 998 -> 1000
  if (lowRound > 100)
    lowRound = std::round(lowRound / 10) * 10;

  // Use high units for distances of 1000 units and over,
  // e.g. 1000m -> 1.0km, 1290m -> 1.3km, 1000ft -> 0.2mi
  if (lowRound >= 1000.0)
  {
    // To high units.
    res = res.To(res.m_units == Units::Meter ? Units::Kilometer : Units::Mile);

    // For distances of 10.0 high units and over round to a whole number, e.g. 9.98 -> 10, 10.9 -> 11
    uint8_t const precision = (std::round(res.m_distance * 10) / 10 >= 10.0) ? 0 : 1;
    return { WithPrecision(res.m_distance, precision), res.m_units };
  }

  res.m_distance = lowRound;
  return res;
}

std::string Distance::ToString() const
{
  if (!IsValid())
    return "";

  return GetDistanceString() + kNarrowNonBreakingSpace + GetUnitsString();
}

std::string DebugPrint(Units units)
{
  // missing yard
  switch (units)
  {
  case Units::Meter: return "m";
  case Units::Kilometer: return "km";
  case Units::Foot: return "ft";
  case Units::Mile: return "mi";
  default: UNREACHABLE();
  }
}

}  // namespace platform
