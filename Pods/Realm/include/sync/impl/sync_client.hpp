////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef REALM_OS_SYNC_CLIENT_HPP
#define REALM_OS_SYNC_CLIENT_HPP

#include "binding_callback_thread_observer.hpp"

#include <realm/sync/client.hpp>
#include <realm/util/scope_exit.hpp>

#include <thread>

#include "sync/sync_manager.hpp"
#include "sync/impl/network_reachability.hpp"

#if NETWORK_REACHABILITY_AVAILABLE
#include "sync/impl/apple/network_reachability_observer.hpp"
#endif

namespace realm {
namespace _impl {

using ReconnectMode = sync::Client::ReconnectMode;

struct SyncClient {
    SyncClient(std::unique_ptr<util::Logger> logger, ReconnectMode reconnect_mode, bool multiplex_sessions, const std::string& user_agent_application_info)
        : m_client(make_client(*logger, reconnect_mode, multiplex_sessions, user_agent_application_info)) // Throws
        , m_logger(std::move(logger))
        , m_thread([this] {
            if (g_binding_callback_thread_observer) {
                g_binding_callback_thread_observer->did_create_thread();
                auto will_destroy_thread = util::make_scope_exit([&]() noexcept {
                    g_binding_callback_thread_observer->will_destroy_thread();
                });
                try {
                    m_client.run(); // Throws
                }
                catch (std::exception const& e) {
                    g_binding_callback_thread_observer->handle_error(e);
                }
            }
            else {
                m_client.run(); // Throws
            }
        }) // Throws
#if NETWORK_REACHABILITY_AVAILABLE
    , m_reachability_observer(none, [=](const NetworkReachabilityStatus status) {
        if (status != NotReachable)
            SyncManager::shared().reconnect();
    })
    {
        if (!m_reachability_observer.start_observing())
            m_logger->error("Failed to set up network reachability observer");
    }
#else
    {
    }
#endif

    void cancel_reconnect_delay() {
        m_client.cancel_reconnect_delay();
    }

    void stop()
    {
        m_client.stop();
        if (m_thread.joinable())
            m_thread.join();
    }

    std::unique_ptr<sync::Session> make_session(std::string path, sync::Session::Config config)
    {
        return std::make_unique<sync::Session>(m_client, std::move(path), std::move(config));
    }


    ~SyncClient()
    {
        stop();
    }

private:
    static sync::Client make_client(util::Logger& logger, ReconnectMode reconnect_mode, bool multiplex_sessions, const std::string& user_agent_application_info)
    {
        sync::Client::Config config;
        config.logger = &logger;
        config.reconnect_mode = std::move(reconnect_mode);
        config.one_connection_per_session = !multiplex_sessions;
        config.user_agent_application_info = user_agent_application_info;
        return sync::Client(std::move(config)); // Throws
    }

    sync::Client m_client;
    const std::unique_ptr<util::Logger> m_logger;
    std::thread m_thread;
#if NETWORK_REACHABILITY_AVAILABLE
    NetworkReachabilityObserver m_reachability_observer;
#endif
};

}
}

#endif // REALM_OS_SYNC_CLIENT_HPP
