#pragma once

#include "fmt/core.h"

#include <span>
#include <string>
#include <string_view>

namespace ErrorHandlingTest {

    inline const std::string c_InitDatabaseQuery = R"(
                DROP VIEW IF EXISTS TeamStandingsView;
                DROP VIEW IF EXISTS TeamResultsView;
                DROP TABLE IF EXISTS Results;
                DROP TABLE IF EXISTS Matches;
                DROP TABLE IF EXISTS Teams;

                CREATE TABLE Teams(
                    teamId     INTEGER       NOT NULL PRIMARY KEY
                  , city       VARCHAR(50)   NOT NULL
                  , name       VARCHAR(50)   NOT NULL
                );

                CREATE TABLE Matches(
                    matchId    INTEGER       NOT NULL PRIMARY KEY
                  , week       INTEGER       NOT NULL
                  , homeTeamId INTEGER       NOT NULL
                  , awayTeamId INTEGER       NOT NULL
                  , FOREIGN KEY (homeTeamId) REFERENCES Teams(teamId)
                  , FOREIGN KEY (awayTeamId) REFERENCES Teams(teamId)
                );

                CREATE TABLE Results(
                    resultId   INTEGER       NOT NULL PRIMARY KEY
                  , matchId    INTEGER       NOT NULL
                  , homeScore  INTEGER       NOT NULL
                  , awayScore  INTEGER       NOT NULL
                  , FOREIGN KEY (matchId) REFERENCES Matches(matchId)
                );

                CREATE VIEW TeamResultsView
                AS
                    SELECT name, location, ourScore, theirScore, ourScore > theirScore AS win, ourScore < theirScore as loss, ourScore = theirScore as tie
                    FROM
                    (
                        SELECT name, 'home' AS location, homeScore as ourScore, awayScore as theirScore
                        FROM Matches
                        INNER JOIN Teams ON Teams.teamId = Matches.homeTeamId
                        INNER JOIN Results ON Results.matchId = Matches.matchId

                        UNION ALL

                        SELECT name, 'away' AS location, awayScore as ourScore, homeScore as theirScore
                        FROM Matches
                        INNER JOIN Teams ON Teams.teamId = Matches.awayTeamId
                        INNER JOIN Results ON Results.matchId = Matches.matchId
                    )
                    ORDER BY name;

                CREATE VIEW TeamStandingsView
                AS
                    SELECT name, SUM(win) AS wins, SUM(loss) AS losses, SUM(tie) AS ties, ((SUM(win) * 1.0) + (SUM(tie) * 0.5)) / COUNT() AS pct
                    FROM TeamResultsView
                    GROUP BY name
                    ORDER BY ((SUM(win) * 1.0) / COUNT()) DESC;
            )";

    inline std::string MakeInsertTeamQuery(std::string_view city, std::string_view name)
    {
        constexpr std::string_view c_InsertTeamTemplate(R"(
            INSERT INTO Teams(city, name) VALUES ('{}', '{}')
        )");

        return fmt::format(c_InsertTeamTemplate, city, name);
    }

    inline std::optional<std::string> TryMakeInsertTeamQuery(std::string_view city, std::string_view name) noexcept
    {
        try
        {
            // There is no non-throwing version of fmt::format, so we will just catch the error and return std::nullopt
            return MakeInsertTeamQuery(city, name);
        }
        catch (const fmt::v8::format_error&)
        {
            return std::nullopt;
        }
    }

    inline std::string MakeInsertMatchQuery(size_t week, std::string_view homeCity, std::string_view awayCity)
    {
        constexpr std::string_view c_InsertMatchTemplate(R"(
                INSERT INTO Matches(week, homeTeamId, awayTeamId)
                    SELECT {}, HomeTeams.teamId, AwayTeams.teamId
                    FROM Teams AS HomeTeams, Teams AS AwayTeams
                    WHERE
                          HomeTeams.City = '{}'
                      AND AwayTeams.City = '{}';
            )");

        return fmt::format(c_InsertMatchTemplate, week, homeCity, awayCity);
    }

    inline std::optional<std::string> TryMakeInsertMatchQuery(size_t week, std::string_view homeCity, std::string_view awayCity) noexcept
    {
        try
        {
            return MakeInsertMatchQuery(week, homeCity, awayCity);
        }
        catch (const fmt::v8::format_error&)
        {
            return std::nullopt;
        }
    }

    inline std::string MakeInsertResultQuery(std::string_view matchId, int homeScore, int awayScore)
    {
        constexpr std::string_view c_InsertResultTemplate(
            R"(
                INSERT INTO Results(matchId, homeScore, awayScore)
                VALUES({}, {}, {});
            )");

        return fmt::format(c_InsertResultTemplate, matchId, homeScore, awayScore);
    }

    inline std::optional<std::string> TryMakeInsertResultQuery(std::string_view matchId, int homeScore, int awayScore) noexcept
    {
        try
        {
            return MakeInsertResultQuery(matchId, homeScore, awayScore);
        }
        catch (const fmt::v8::format_error&)
        {
            return std::nullopt;
        }
    }

    inline std::string MakeGetMatchesQuery(size_t week)
    {
        constexpr std::string_view c_MatchesQueryTemplate(R"(
                SELECT matchId
                FROM Matches
                WHERE week = {};
            )");

        return fmt::format(c_MatchesQueryTemplate, week);
    }

    inline std::optional<std::string> TryMakeGetMatchesQuery(size_t week) noexcept
    {
        try
        {
            return MakeGetMatchesQuery(week);
        }
        catch (const fmt::v8::format_error&)
        {
            return std::nullopt;
        }
    }

    inline const std::string c_ListTeamsQuery = R"(SELECT city, name FROM Teams;)";

    inline const std::string c_LeagueScheduleQuery = R"(
                SELECT week, HomeTeams.city AS home, AwayTeams.city AS away
                FROM Matches
                INNER JOIN Teams AS HomeTeams ON HomeTeams.teamId = Matches.homeTeamId
                INNER JOIN Teams AS AwayTeams ON AwayTeams.teamId = Matches.awayTeamId
                ORDER BY week;
            )";

    inline const std::string c_LeagueResultsQuery = R"(
                SELECT week, HomeTeams.name AS home, AwayTeams.name AS away, homeScore, awayScore
                FROM Matches
                INNER JOIN Teams AS HomeTeams ON HomeTeams.teamId = Matches.homeTeamId
                INNER JOIN Teams AS AwayTeams ON AwayTeams.teamId = Matches.awayTeamId
                INNER JOIN Results ON Results.matchId = Matches.matchId
                ORDER BY week;
            )";

    inline const std::string c_TeamStandingsQuery = R"(
                SELECT name, wins, losses, ties, ROUND(pct, 3) as pct
                FROM TeamStandingsView;
            )";

} // namespace ErrorHandlingTest
