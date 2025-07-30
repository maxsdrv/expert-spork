#pragma once

#include "types/Macro.h"

#include <boost/locale.hpp>

#include <vector>

#define TR_0(MSG, ...) (MSG).str()
#define TR_1(MSG, PARAM, ...) TR_0(MSG % PARAM, __VA_ARGS__)
#define TR_2(MSG, PARAM, ...) TR_1(MSG % PARAM, __VA_ARGS__)
#define TR_3(MSG, PARAM, ...) TR_2(MSG % PARAM, __VA_ARGS__)
#define TR_4(MSG, PARAM, ...) TR_3(MSG % PARAM, __VA_ARGS__)
#define TR_5(MSG, PARAM, ...) TR_4(MSG % PARAM, __VA_ARGS__)

/** Common form of translate */
#define TR_N(DOMAIN, STRING, ...)   \
    VA_MACRO_NAME(TR_, __VA_ARGS__) \
    (boost::locale::format(boost::locale::translate(STRING).str(DOMAIN)), __VA_ARGS__)

/** Plural form of translate */
#define TR_PL_N(DOMAIN, STRING, STRING_PL, COUNT, ...)                                      \
    VA_MACRO_NAME(TR_, __VA_ARGS__)                                                         \
    (boost::locale::format(boost::locale::translate(STRING, STRING_PL, COUNT).str(DOMAIN)), \
     __VA_ARGS__)

/** Translate with context */
#define TR_CTX_N(DOMAIN, CONTEXT, STRING, ...) \
    VA_MACRO_NAME(TR_, __VA_ARGS__)            \
    (boost::locale::format(boost::locale::translate(CONTEXT, STRING).str(DOMAIN)), __VA_ARGS__)

namespace mpkit::translate
{

/**
 * Link precompiled .mo modules from @path
 */
void installBoostTranslation(const std::string& path, const std::vector<std::string>& domains);

/**
 * Link all .mo modules from @path
 */
std::vector<std::string> installBoostTranslation(const std::string& path);

} // namespace mpkit::translate
