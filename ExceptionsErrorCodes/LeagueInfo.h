#pragma once

#include "Sqlite3Helpers.h"

#include <array>
#include <optional>
#include <string>
#include <string_view>

// Defines the data types and constants for defining the sports league

namespace ErrorHandlingTest {

    struct TeamInfo
    {
        constexpr TeamInfo(std::string_view city, std::string_view name) : City(city), Name(name)
        {
        }

        constexpr TeamInfo(const Sqlite3Helpers::Sqlite3Row& row) : TeamInfo(row.at("city").value(), row.at("name").value())
        {
        }

        static std::optional<TeamInfo> FromRow(const Sqlite3Helpers::Sqlite3Row& row)
        {
            if (row.find("city") == row.end() || row.find("name") == row.end())
            {
                return std::nullopt;
            }

            return TeamInfo(row);
        }

        const std::string_view City;
        const std::string_view Name;
    };

    struct Match
    {
        constexpr Match(int week, std::string_view home, std::string_view away) : Week(week), Home(home), Away(away)
        {
        }

        constexpr Match(const Sqlite3Helpers::Sqlite3Row& row)
            : Match(std::stoi(row.at("week").value()), row.at("home").value(), row.at("away").value())
        {
        }

        static std::optional<Match> FromRow(const Sqlite3Helpers::Sqlite3Row& row)
        {
            if (row.find("week") == row.end() || row.find("home") == row.end() || row.find("away") == row.end())
            {
                return std::nullopt;
            }

            return Match(row);
        }

        const int              Week;
        const std::string_view Home;
        const std::string_view Away;
    };

    struct Result
    {
        constexpr Result(int week, std::string_view home, std::string_view away, int homeScore, int awayScore)
            : Week(week)
            , Home(home)
            , Away(away)
            , HomeScore(homeScore)
            , AwayScore(awayScore)
        {
        }

        constexpr Result(const Sqlite3Helpers::Sqlite3Row& row)
            : Result(
                std::stoi(row.at("week").value()),
                row.at("home").value(),
                row.at("away").value(),
                std::stoi(row.at("homeScore").value()),
                std::stoi(row.at("awayScore").value()))
        {
        }

        static std::optional<Result> FromRow(const Sqlite3Helpers::Sqlite3Row& row)
        {
            if (row.find("week") == row.end() || row.find("home") == row.end() || row.find("away") == row.end()
                || row.find("homeScore") == row.end() || row.find("awayScore") == row.end())
            {
                return std::nullopt;
            }

            return Result(row);
        }

        const int              Week;
        const std::string_view Home;
        const std::string_view Away;
        const int              HomeScore;
        const int              AwayScore;
    };

    struct TeamRecord
    {
        constexpr TeamRecord(std::string_view name, int wins, int losses, int ties, double percent)
            : Name(name)
            , Wins(wins)
            , Losses(losses)
            , Ties(ties)
            , Percent(percent)
        {
        }

        constexpr TeamRecord(const Sqlite3Helpers::Sqlite3Row& row)
            : TeamRecord(
                row.at("name").value(),
                std::stoi(row.at("wins").value()),
                std::stoi(row.at("losses").value()),
                std::stoi(row.at("ties").value()),
                std::stod(row.at("pct").value()))
        {
        }

        static std::optional<TeamRecord> FromRow(const Sqlite3Helpers::Sqlite3Row& row)
        {
            if (row.find("name") == row.end() || row.find("wins") == row.end() || row.find("losses") == row.end()
                || row.find("ties") == row.end() || row.find("percent") == row.end())
            {
                return std::nullopt;
            }

            return TeamRecord(row);
        }

        const std::string_view Name;
        const int              Wins;
        const int              Losses;
        const int              Ties;
        const double           Percent;
    };

    inline constexpr std::array c_Teams = {

        TeamInfo("Arizona", "Cardinals"),
        TeamInfo("Los Angeles", "Rams"),
        TeamInfo("San Francisco", "49ers"),
        TeamInfo("Seattle", "Seahawks")

    };

    inline constexpr size_t c_SeasonLength = 6;

} // namespace ErrorHandlingTest