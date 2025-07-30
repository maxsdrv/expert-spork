#include "mpk/rest/RestApiClientHelper.h"

namespace mpk::rest
{

RestApiClientHelper::RestApiClientHelper(ClientSingle restClient) : m_restClient{restClient}
{
}

std::string RestApiClientHelper::errorCodeTag()
{
    return "error";
}

std::string RestApiClientHelper::textDataTag()
{
    return "data";
}

} // namespace mpk::rest
