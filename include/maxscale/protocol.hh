/*
 * Copyright (c) 2018 MariaDB Corporation Ab
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
#pragma once

/**
 * @file protocol.hh
 *
 * The protocol module interface definition.
 */

#include <maxscale/ccdefs.hh>

#include <maxbase/jansson.h>
#include <maxscale/buffer.hh>


struct DCB;
class SERVER;
struct MXS_SESSION;

/**
 * Protocol module API
 */
struct MXS_PROTOCOL
{
    /**
     * EPOLLIN handler, used to read available data from network socket
     *
     * @param dcb DCB to read from
     *
     * @return 1 on success, 0 on error
     */
    int32_t (* read)(DCB* dcb);

    /**
     * Write data to a network socket
     *
     * @param dcb    DCB to write to
     * @param buffer Buffer to write
     *
     * @return 1 on success, 0 on error
     */
    int32_t (* write)(DCB* dcb, GWBUF* buffer);

    /**
     * EPOLLOUT handler, used to write buffered data
     *
     * @param dcb DCB to write to
     *
     * @return 1 on success, 0 on error
     *
     * @note Currently the return value is ignored
     */
    int32_t (* write_ready)(DCB* dcb);

    /**
     * EPOLLERR handler
     *
     * @param dcb DCB for which the error occurred
     *
     * @return 1 on success, 0 on error
     *
     * @note Currently the return value is ignored
     */
    int32_t (* error)(DCB* dcb);

    /**
     * EPOLLHUP and EPOLLRDHUP handler
     *
     * @param dcb DCB for which the hangup occurred
     *
     * @return 1 on success, 0 on error
     *
     * @note Currently the return value is ignored
     */
    int32_t (* hangup)(DCB* dcb);

    /**
     * Accept a connection, only for client side protocol modules
     *
     * @param dcb The client DCB
     *
     * @return 1 on success, 0 on error
     *
     * @note Currently the return value is ignored
     */
    int32_t (* accept)(DCB* client_dcb);

    /**
     * Connect to a server, only for backend side protocol modules
     *
     * @param dcb     DCB to connect
     * @param server  Server where the connection is made
     * @param session The session where the DCB should be linked to
     *
     * @return The opened file descriptor or DCBFD_CLOSED on error
     */
    int32_t (* connect)(DCB* dcb, SERVER* server, MXS_SESSION* session);

    /**
     * Free protocol data allocated in the connect handler
     *
     * @param dcb DCB to close
     *
     * @return 1 on success, 0 on error
     *
     * @note Currently the return value is ignored
     */
    int32_t (* close)(DCB* dcb);

    /**
     * Perform user re-authentication
     *
     * @param dcb     DCB to re-authenticate
     * @param server  Server where the DCB is connected
     * @param session The session for the DCB
     * @param buffer  The buffer containing the original re-authentication request
     *
     * @return 1 on success, 0 on error
     *
     * @note Currently the return value is ignored
     */
    int32_t (* auth)(DCB* dcb, SERVER* server, MXS_SESSION* session, GWBUF* buffer);

    /**
     * Returns the name of the default authenticator module for this protocol
     *
     * @return The name of the default authenticator
     */
    char* (* auth_default)();

    /**
     * Handle connection limits
     *
     * @param dcb   DCB to handle
     * @param limit Maximum number of connections
     *
     * @return 1 on success, 0 on error
     *
     * @note Currently the return value is ignored
     */
    int32_t (* connlimit)(DCB* dcb, int limit);

    /**
     * Check if the connection has been fully established, used by connection pooling
     *
     * @param dcb DCB to check
     *
     * @return True if the connection is fully established and can be pooled
     */
    bool (* established)(DCB*);

    /**
     * Provide JSON formatted diagnostics about a DCB
     *
     * @param dcb DCB to diagnose
     *
     * @return JSON representation of the DCB
     */
    json_t* (* diagnostics_json)(DCB* dcb);

    /**
     * Get rejection message
     *
     * The protocol should return an error indicating that access to MaxScale has been temporarily suspended.
     *
     * @param host The host that is blocked
     *
     * @return A buffer containing the error message
     */
    GWBUF* (* reject)(const char* host);
};

/**
 * The MXS_PROTOCOL version data. The following should be updated whenever
 * the MXS_PROTOCOL structure is changed. See the rules defined in modinfo.h
 * that define how these numbers should change.
 */
#define MXS_PROTOCOL_VERSION {2, 1, 0}

/**
 * Specifies capabilities specific for protocol.
 *
 * @see enum routing_capability
 *
 * @note The values of the capabilities here *must* be between 0x010000000000
 *       and 0x800000000000, that is, bits 40 to 47.
 */
enum protocol_capability_t
{
    PCAP_TYPE_NONE = 0x0    // TODO: remove once protocol capabilities are defined
};
