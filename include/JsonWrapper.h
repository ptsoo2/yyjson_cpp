#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <map>

#include <memory>
#include <functional>

// common trait
template<typename TType, typename TDecayType = std::remove_cvref_t<TType>>
concept SignedIntegerTrait =
	std::is_integral_v<TDecayType> == true
	&& std::is_unsigned_v<TDecayType> == false
	;
template<typename TType, typename TDecayType = std::remove_cvref_t<TType>>
concept UnSignedIntegerTrait =
	std::is_integral_v<TDecayType> == true
	&& std::is_unsigned_v<TDecayType> == true
	;
template<typename TType, typename TDecayType = std::remove_cvref_t<TType>>
concept FloatingPointTrait =
	std::is_floating_point_v<TDecayType> == true
	;

#include "yyjson.h"
#include "JsonReader.h"
#include "JsonWriter.h"
