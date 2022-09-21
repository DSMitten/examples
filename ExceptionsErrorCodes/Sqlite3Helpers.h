#pragma once

#include "sqlite/sqlite3.h"

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Sqlite3Helpers {
    struct Sqlite3Deleter
    {
        void operator()(sqlite3* db)
        {
            sqlite3_close(db);
        }
    };

    using unique_sqlite3 = std::unique_ptr<sqlite3, Sqlite3Deleter>;

    class Sqlite3ErrorMessage
    {
    public:
        ~Sqlite3ErrorMessage()
        {
            if (m_pErrorMessage != nullptr)
            {
                sqlite3_free(m_pErrorMessage);
            }
        }

        std::string string()
        {
            return std::string(m_pErrorMessage);
        }

        operator char**()
        {
            return &m_pErrorMessage;
        }

    private:
        char* m_pErrorMessage = nullptr;
    };

    using Sqlite3Row = std::unordered_map<std::string, std::optional<std::string>>;
    using Sqlite3Rows = std::vector<Sqlite3Row>;

    inline int ProcessSqlite3Row(void* pOutput, int columnCount, char** ppValues, char** ppNames) noexcept
    {
        Sqlite3Row row;
        for (int index = 0; index < columnCount; index++)
        {
            if (ppValues[index] == nullptr)
            {
                row[ppNames[index]] = std::nullopt;
            }
            else
            {
                row[ppNames[index]] = ppValues[index];
            }
        }

        reinterpret_cast<Sqlite3Rows*>(pOutput)->emplace_back(std::move(row));

        return 0;
    }

    namespace WithExceptions {
        using namespace ::Sqlite3Helpers;

        inline unique_sqlite3 OpenSqlite3(const char* filename)
        {
            sqlite3* db;

            int errorCode = sqlite3_open(filename, &db);
            if (errorCode)
            {
                throw std::system_error(std::error_code(errorCode, std::generic_category()), sqlite3_errmsg(db));
            }

            return unique_sqlite3(db);
        }

        inline Sqlite3Rows ExecuteSql(sqlite3* db, const std::string& sql)
        {
            Sqlite3Rows         output;
            Sqlite3ErrorMessage errorMessage;

            int errorCode = sqlite3_exec(db, sql.c_str(), ProcessSqlite3Row, &output, errorMessage);

            if (errorCode != SQLITE_OK)
            {
                throw std::system_error(std::error_code(errorCode, std::generic_category()), errorMessage.string());
            }

            return output;
        }
    } // namespace WithExceptions

    namespace WithOptionals {
        using namespace ::Sqlite3Helpers;

        inline unique_sqlite3 OpenSqlite3(const char* filename)
        {
            sqlite3* db;

            int retval = sqlite3_open(filename, &db);
            if (retval)
            {
                return nullptr;
            }

            return unique_sqlite3(db);
        }

        inline std::optional<Sqlite3Rows> ExecuteSql(sqlite3* db, const std::string& sql)
        {
            Sqlite3Rows output;

            int errorCode = sqlite3_exec(db, sql.c_str(), ProcessSqlite3Row, &output, nullptr);

            if (errorCode != SQLITE_OK)
            {
                return std::nullopt;
            }

            return output;
        }
    } // namespace WithOptionals
} // namespace Sqlite3Helpers