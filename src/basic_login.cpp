//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

#if !defined(HELBREATHX)
uint64_t CGame::check_account_auth(CClient * client, std::string & account, std::string & pass)
{
    try
    {
        std::shared_lock<std::shared_mutex> l(game_sql_mtx);
        // todo: use a proper hashing method
        pqxx::work txn{ *pq_game };
        pqxx::row r{ txn.exec_params1("SELECT * FROM accounts WHERE email=$1 AND pass=$2 LIMIT 1", account, pass) };
        txn.commit();

        if (r["pass"].as<std::string>() == pass)
        {
            return r["id"].as<int64_t>();
        }
        throw std::exception(std::format("Wrong pass [{}]", account).c_str());
    }
    catch (pqxx::unexpected_rows &)
    {
#if defined(CREATE_ACCOUNT)
        log->info(fmt::format("Creating account [{}]", account));
        try
        {
            std::shared_lock<std::shared_mutex> l(game_sql_mtx);
            pqxx::work txn{ *pq_game };
            pqxx::row r{ txn.exec_params1("INSERT INTO accounts (email, pass) VALUES ($1, $2) RETURNING *", account, pass) };
            txn.commit();
            return r["id"].as<uint64_t>();
        }
        catch (std::exception & ex)
        {
            throw std::exception(std::format("Error creating account [{}] - {}", account, ex.what()).c_str());
        }
#else
        throw std::exception(std::format("Account doesn't exist [{}]", account).c_str());
#endif
    }
    catch (std::exception & ex)
    {
        throw std::exception(std::format("Error querying login attempt for account [{}] - {}", account, ex.what()).c_str());
    }
}
#endif
