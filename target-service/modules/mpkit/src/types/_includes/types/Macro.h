/** @file
 * @brief  Macro expansion set
 *
 *
 * $Id: $
 */

#pragma once

// This macros accept up to 5 params
// One can easily expand this for more
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(0, ##__VA_ARGS__, 5, 4, 3, 2, 1, 0)
#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N
#define VA_MACRO_NAME(NAME, ...) CONCAT(NAME, VA_NUM_ARGS(__VA_ARGS__))

#define VA_LAST_ARG_1(ARG, ...) ARG
#define VA_LAST_ARG_2(ARG, ...) VA_LAST_ARG_1(__VA_ARGS__)
#define VA_LAST_ARG_3(ARG, ...) VA_LAST_ARG_2(__VA_ARGS__)
#define VA_LAST_ARG_4(ARG, ...) VA_LAST_ARG_3(__VA_ARGS__)
#define VA_LAST_ARG_5(ARG, ...) VA_LAST_ARG_4(__VA_ARGS__)
#define VA_LAST_ARG(...) VA_MACRO_NAME(VA_LAST_ARG_, __VA_ARGS__)(__VA_ARGS__)

#define CONCAT(S1, S2) CONCAT_(S1, S2)
#define CONCAT_(S1, S2) CONCAT__(S1, S2)
#define CONCAT__(S1, S2) S1##S2 // NOLINT (bugprone-reserved-identifier)
#define STRING(S) STRING_(S)
#define STRING_(S) STRING__(S)
#define STRING__(S) #S // NOLINT (bugprone-reserved-identifier)

#define UNUSED(x) (void)x
