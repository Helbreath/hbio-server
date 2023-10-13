//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#include <ixwebsocket/IXHttpClient.h>

using json = nlohmann::json;

bool CGame::check_account_auth(CClient * client, std::string & account, std::string & pass, int64_t & account_id)
{
    if (client && client->logged_in == true) return true;
    try
    {
        ix::HttpClient httpClient;
        std::string url = login_auth_url;
        ix::HttpRequestArgsPtr args = httpClient.createRequest(url, ix::HttpClient::kPost);
        ix::HttpResponsePtr out;

        json obj;
        obj["email"] = account;
        obj["pass"] = pass;
        obj["key"] = login_auth_key;
        out = httpClient.post(
            url,
            obj.dump(),
            args
        );

        uint64_t external_member_id;

        if (out->statusCode == 244)
        {
            log->info(fmt::format("Successful login [{}]", account));

            json response = json::parse(out->body);
            external_member_id = response["forum_member_id"].get<uint64_t>();
            pass = response["hash"].get<std::string>();
        }
        else
        {
            if (out->body == "badaccount")
            {
                log->critical("Account doesn't exist [{}]", account);
                return false;
            }
            if (out->body == "badpass")
            {
                log->critical("Wrong pass [{}]", account);
                return false;
            }
            if (out->body == "Invalid Access 1" || out->body == "Invalid Access 2")
            {
                log->critical("Configuration incorrect on login script");
                return false;
            }
            log->critical("Unknown error during login call");
            return false;
        }

        try
        {
            std::shared_lock<std::shared_mutex> l(game_sql_mtx);
            pqxx::work txn{ *pq_game };
            pqxx::row r{ txn.exec_params1("SELECT * FROM accounts WHERE forum_member_id=$1 LIMIT 1", external_member_id) };
            txn.commit();

            account_id = r["id"].as<int64_t>();
        }
        catch (pqxx::unexpected_rows &)
        {
            std::shared_lock<std::shared_mutex> l(game_sql_mtx);
            // no rows exist - create new account
            pqxx::work txn{ *pq_game };
            pqxx::row r{ txn.exec_params1(
                "INSERT INTO accounts (email, forum_member_id) VALUES ($1, $2) RETURNING *",
                pq_game->quote(account),
                external_member_id
            ) };
            txn.commit();

            account_id = r["id"].as<int64_t>();
            return true;
        }
        return true;
    }
    catch (std::exception & ex)
    {
        log->critical("Error querying login attempt for account [{}] - {}", account, ex.what());
    }
    return false;
}
