/**
 * @file
 * @brief Test for BaseException
 *
 *
 * $Id: $
 */

#include "exception/General.h"

#include <gtest/gtest.h>

namespace appkit
{

namespace coriolis
{

namespace tests
{

void throwBaseException()
{
    BOOST_THROW_EXCEPTION(
        exception::General() << exception::ExceptionInfo("test id"));
}

TEST(BaseExceptionTest, testAsStdException)
{
    EXPECT_THROW(throwBaseException(), std::exception);
}

TEST(BaseExceptionTest, testAsBoostException)
{
    EXPECT_THROW(throwBaseException(), boost::exception);
}

TEST(BaseExceptionTest, testErrorInfoId)
{
    try
    {
        throwBaseException();
    }
    catch (boost::exception& ex)
    {
        EXPECT_EQ(
            "test id", *boost::get_error_info<exception::ExceptionInfo>(ex));
    }
}

} // namespace tests

} // namespace coriolis

} // namespace appkit
