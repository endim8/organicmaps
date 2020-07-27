#include "platform/localization.hpp"

namespace platform
{
std::string GetLocalizedTypeName(std::string const & type) { return type; }

std::string GetLocalizedBrandName(std::string const & brand) { return brand; }

std::string GetLocalizedString(std::string const & key) { return key; }

std::string GetCurrencySymbol(std::string const & currencyCode) { return currencyCode; }
}  // namespace platform
