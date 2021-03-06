/*
 * Copyright (c) 2016 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl11.
 *
 * Change Date: 2024-02-10
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */

// To ensure that ss_info_assert asserts also when builing in non-debug mode.
#if !defined (SS_DEBUG)
#define SS_DEBUG
#endif
#if defined (NDEBUG)
#undef NDEBUG
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <maxscale/maxscale_test.h>
#include <maxscale/paths.h>
#include <maxbase/alloc.h>

#include "../internal/service.hh"
#include "test_utils.hh"
#include "../config.cc"

/**
 * test1    Allocate a service and do lots of other things
 *
 */
static int test1()
{
    Service* service;
    MXS_SESSION* session;
    DCB* dcb;
    int result;
    int argc = 3;

    init_test_env(NULL);

    set_libdir(MXS_STRDUP_A("../../modules/authenticator/MySQLAuth/"));
    load_module("mysqlauth", MODULE_AUTHENTICATOR);
    set_libdir(MXS_STRDUP_A("../../modules/protocol/MySQL/mariadbclient/"));
    load_module("mariadbclient", MODULE_PROTOCOL);
    set_libdir(MXS_STRDUP_A("../../modules/routing/readconnroute/"));
    load_module("readconnroute", MODULE_ROUTER);

    MXS_CONFIG_PARAMETER parameters;
    parameters.set(CN_MAX_RETRY_INTERVAL, "10s");
    parameters.set(CN_CONNECTION_TIMEOUT, "10s");
    parameters.set(CN_NET_WRITE_TIMEOUT, "10s");
    /* Service tests */
    fprintf(stderr,
            "testservice : creating service called MyService with router nonexistent");
    service = service_alloc("MyService", "non-existent", &parameters);
    mxb_assert_message(NULL == service, "New service with invalid router should be null");
    mxb_assert_message(0 == service_isvalid(service), "Service must not be valid after incorrect creation");
    fprintf(stderr, "\t..done\nValid service creation, router testroute.");
    service = service_alloc("MyService", "readconnroute", &parameters);

    mxb_assert_message(NULL != service, "New service with valid router must not be null");
    mxb_assert_message(0 != service_isvalid(service), "Service must be valid after creation");
    mxb_assert_message(0 == strcmp("MyService", service->name()), "Service must have given name");
    fprintf(stderr, "\t..done\nAdding protocol testprotocol.");

    MXS_CONFIG_PARAMETER listener_params;
    listener_params.set(CN_ADDRESS, "localhost");
    listener_params.set(CN_PORT, "9876");
    listener_params.set(CN_PROTOCOL, "mariadbclient");
    listener_params.set(CN_SERVICE, service->name());

    mxb_assert_message(Listener::create("TestProtocol", "mariadbclient", listener_params),
                       "Add Protocol should succeed");
    mxb_assert_message(service_find_listener(service, "", "localhost", 9876),
                       "Service should have new protocol as requested");

    return 0;
}

int main(int argc, char** argv)
{
    int result = 0;

    result += test1();

    exit(result);
}
