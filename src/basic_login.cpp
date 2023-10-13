//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

#if !defined(HELBREATHX)
bool CGame::check_account_auth(CClient * client, std::string & account, std::string & pass, int64_t & account_id)
{
    try
    {
        std::shared_lock<std::shared_mutex> l(login_sql_mtx);
        pqxx::work txn{ *pq_login };
        pqxx::row r{ txn.exec_params1("SELECT * FROM accounts WHERE email=$1 LIMIT 1", pq_login->quote(account)) };
        txn.commit();

        // todo: use a proper hashing method
        if (r["pass"].as<std::string>() == pass)
        {
            return true;
        }
        log->info(fmt::format("Wrong pass [{}]", account));
        return false;
    }
    catch (pqxx::unexpected_rows &)
    {
        log->info(fmt::format("Creating account [{}]", account));
        try
        {
            std::shared_lock<std::shared_mutex> l(login_sql_mtx);
            pqxx::work txn{ *pq_login };
            pqxx::result r{ txn.exec_params(
                "INSERT INTO accounts (email, pass) VALUES ($1, $2)",
                pq_login->quote(account),
                pq_login->quote(pass)
            ) };
            txn.commit();
            return true;
        }
        catch (std::exception & ex)
        {
            log->critical(fmt::format("Error creating account [{}] - {}", account, ex.what()));
        }
    }
    catch (std::exception & ex)
    {
        log->critical(fmt::format("Error querying login attempt for account [{}] - {}", account, ex.what()));
    }
    return false;
}
#endif
