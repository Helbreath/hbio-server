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
        std::shared_lock<std::shared_mutex> l(login_sql_mtx);
        pqxx::work txn{ *pq_login };
        pqxx::row r{ txn.exec_params1("SELECT * FROM accounts WHERE email=$1 LIMIT 1", account) };
        txn.commit();

        // todo: use a proper hashing method
        if (r["pass"].as<std::string>() == pass)
        {
            return r["id"].as<int64_t>();
        }
        throw std::exception(std::format("Wrong pass [{}]", account).c_str());
    }
    catch (pqxx::unexpected_rows &)
    {
        log->info(fmt::format("Creating account [{}]", account));
        try
        {
            std::shared_lock<std::shared_mutex> l(login_sql_mtx);
            pqxx::work txn{ *pq_login };
            pqxx::result r{ txn.exec_params("INSERT INTO accounts (email, pass) VALUES ($1, $2)", account, pass) };
            txn.commit();
            return r["id"].as<int64_t>();
        }
        catch (std::exception & ex)
        {
            throw std::exception(std::format("Error creating account [{}] - {}", account, ex.what()).c_str());
        }
    }
    catch (std::exception & ex)
    {
        throw std::exception(std::format("Error querying login attempt for account [{}] - {}", account, ex.what()).c_str());
    }
}
#endif
