// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

#include "stdafx.h"
#include <cpprest\basic_types.h>
#include <cpprest\asyncrt_utils.h>
#include "http_sender.h"
#include "signalrclient\web_exception.h"

using namespace signalr;

struct web_request_stub
{
    unsigned short m_status_code;
    utility::string_t m_reason_code;
    utility::string_t m_response_body;
    utility::string_t m_method;
    utility::string_t m_user_agent_string;

    void set_method(const utility::string_t &method)
    {
        m_method = method;
    }

    void set_user_agent(const utility::string_t &user_agent_string)
    {
        m_user_agent_string = user_agent_string;
    }

    pplx::task<web_response> get_response()
    {
        return pplx::task_from_result<web_response>(
            web_response{ m_status_code, m_reason_code, pplx::task_from_result<utility::string_t>(m_response_body) });
    }
};

TEST(http_sender_get_response, request_prepared_and_sent)
{
    utility::string_t response_body{ _XPLATSTR("response body") };
    web_request_stub request{ (unsigned short)200, _XPLATSTR("OK"), response_body };

    ASSERT_EQ(response_body, http_sender::get<web_request_stub>(request).get());
}


TEST(http_sender_get_response, exception_thrown_if_status_code_not_200)
{
    utility::string_t response_phrase(_XPLATSTR("Custom Not Found"));
    web_request_stub request{ (unsigned short)404, response_phrase };

    try
    {
        http_sender::get(request).get();
        ASSERT_TRUE(false); // exception not thrown 
    }
    catch (const web_exception &e)
    {
        ASSERT_EQ(
            _XPLATSTR("web exception: 404 ") + response_phrase,
            utility::conversions::print_string(e.what()));
    }
}