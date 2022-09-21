#include "ErrorHandlingTest.h"
#include "LeagueInfo.h"
#include "SqlQueries.h"
#include "Sqlite3Helpers.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <format>
#include <sstream>
#include <string>
#include <string_view>

using namespace Sqlite3Helpers::WithOptionals;

namespace ErrorHandlingTest::UseOptionals {
    namespace {

        // -------------------------------------------------------------------------
        // Database operations
        // -------------------------------------------------------------------------
        std::optional<unique_sqlite3> OpenTestDatabase()
        {
            return OpenSqlite3("extest.db");
        }

        bool InitDatabase(sqlite3* db)
        {
            return ExecuteSql(db, c_InitDatabaseQuery) != std::nullopt;
        }

        bool AddTeam(sqlite3* db, std::string_view city, std::string_view name)
        {
            std::optional<std::string> sql = TryMakeInsertTeamQuery(city, name);
            if (!sql.has_value())
            {
                return false;
            }

            return ExecuteSql(db, sql.value()) != std::nullopt;
        }

        bool AddMatch(sqlite3* db, size_t week, std::string_view homeCity, std::string_view awayCity)
        {
            std::optional<std::string> sql = TryMakeInsertMatchQuery(week, homeCity, awayCity);
            if (!sql.has_value())
            {
                return false;
            }

            return ExecuteSql(db, sql.value()) != std::nullopt;
        }

        bool AddResult(sqlite3* db, std::string_view matchId, int homeScore, int awayScore)
        {
            std::optional<std::string> sql = TryMakeInsertResultQuery(matchId, homeScore, awayScore);
            if (!sql.has_value())
            {
                return false;
            }

            return ExecuteSql(db, sql.value()) != std::nullopt;
        }

        // ----------------------------------------------------------------------------
        // Populating teams table
        // ----------------------------------------------------------------------------
        bool AddTeams(sqlite3* db)
        {
            for (const TeamInfo& teamInfo : c_Teams)
            {
                if (!AddTeam(db, teamInfo.City, teamInfo.Name))
                {
                    return false;
                }
            }

            return true;
        }

        // ----------------------------------------------------------------------------
        // Populating matches table
        // ----------------------------------------------------------------------------
        size_t PickRandomTeam(std::vector<size_t>& pickedTeams)
        {
            size_t teamIndex = c_Teams.size();
            while (teamIndex == c_Teams.size() || std::find(pickedTeams.begin(), pickedTeams.end(), teamIndex) != pickedTeams.end())
            {
                size_t randomValue = static_cast<size_t>(std::rand());
                teamIndex = randomValue % c_Teams.size();
            }
            pickedTeams.push_back(teamIndex);
            return teamIndex;
        }

        bool CreateSchedule(sqlite3* db)
        {
            for (size_t week = 1; week <= c_SeasonLength; week++)
            {
                std::vector<size_t> pickedTeams {};
                while (pickedTeams.size() < c_Teams.size())
                {
                    const size_t homeIndex = PickRandomTeam(pickedTeams); // Pick any team
                    const size_t awayIndex = PickRandomTeam(pickedTeams); // Pick any team other than the home team
                    if (!AddMatch(db, week, c_Teams[homeIndex].City, c_Teams[awayIndex].City))
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        // ----------------------------------------------------------------------------
        // Populating results table
        // ----------------------------------------------------------------------------
        int SimulateScore()
        {
            int touchdowns = static_cast<int>(std::rand()) % 5;
            int fgs = static_cast<int>(std::rand()) % 3;
            return touchdowns * 7 + fgs * 3;
        }

        bool PlayWeek(sqlite3* db, size_t week, bool insertFailure)
        {
            std::optional<std::string> sql = TryMakeGetMatchesQuery(week);
            if (!sql.has_value())
            {
                return false;
            }

            if (insertFailure)
            {
                sql = "Hurr durr";
            }

            std::optional<Sqlite3Rows> rowsOpt = ExecuteSql(db, sql.value());
            if (!rowsOpt.has_value())
            {
                return false;
            }

            for (const Sqlite3Row& row : rowsOpt.value())
            {
                int homeScore = SimulateScore();
                int awayScore = SimulateScore();
                AddResult(db, row.at("matchId").value(), homeScore, awayScore);
            }

            return true;
        }

        bool PlaySchedule(sqlite3* db, bool insertFailure)
        {
            for (size_t week = 1; week <= c_SeasonLength; week++)
            {
                if (!PlayWeek(db, week, insertFailure))
                {
                    return false;
                }
            }

            return true;
        }

        // ----------------------------------------------------------------------------
        // Writing teams
        // ----------------------------------------------------------------------------
        bool ListTeams(sqlite3* db, std::stringstream& out)
        {
            std::optional<Sqlite3Rows> rows = ExecuteSql(db, c_ListTeamsQuery);
            if (!rows.has_value())
            {
                return false;
            }

            out << "\nTeams:\n";
            for (const Sqlite3Row& row : rows.value())
            {
                const std::optional<TeamInfo> teamInfoOpt = TeamInfo::FromRow(row);
                if (!teamInfoOpt.has_value())
                {
                    return false;
                }

                const TeamInfo& teamInfo = teamInfoOpt.value();

                out << teamInfo.City << " " << teamInfo.Name << "\n";
            }

            return true;
        }

        // ----------------------------------------------------------------------------
        // Writing schedule
        // ----------------------------------------------------------------------------
        bool WriteSchedule(sqlite3* db, std::stringstream& out)
        {
            std::optional<Sqlite3Rows> rows = ExecuteSql(db, c_LeagueScheduleQuery);
            if (!rows.has_value())
            {
                return false;
            }

            out << "\nSchedule:\n";
            int week = -1;
            for (const Sqlite3Row& row : rows.value())
            {
                const std::optional<Match> matchOpt = Match::FromRow(row);

                if (!matchOpt.has_value())
                {
                    return false;
                }

                const Match& match = matchOpt.value();

                if (week != match.Week)
                {
                    week = match.Week;
                    out << "Week " << week << ":\n";
                }

                out << "    " << match.Away << " at " << match.Home << "\n";
            }

            return true;
        }

        // ----------------------------------------------------------------------------
        // Writing scores
        // ----------------------------------------------------------------------------
        void WriteResult(std::string_view team1, int team1Score, std::string_view team2, int team2Score, std::stringstream& out)
        {
            out << "    " << team1 << ": " << team1Score << ", " << team2 << ": " << team2Score << "\n";
        }

        bool WriteResults(sqlite3* db, std::stringstream& out)
        {
            std::optional<Sqlite3Rows> rows = ExecuteSql(db, c_LeagueResultsQuery);
            if (!rows.has_value())
            {
                return false;
            }

            out << "\nResults:\n";
            int week = -1;
            for (const Sqlite3Row& row : rows.value())
            {
                const std::optional<Result> resultOpt = Result::FromRow(row);
                if (!resultOpt.has_value())
                {
                    return false;
                }

                const Result& result = resultOpt.value();

                if (week != result.Week)
                {
                    week = result.Week;
                    out << "Week " << week << ":\n";
                }

                WriteResult(result.Away, result.AwayScore, result.Home, result.HomeScore, out);
            }

            return true;
        }

        // ----------------------------------------------------------------------------
        // Writing team standings
        // ----------------------------------------------------------------------------
        bool WriteStandings(sqlite3* db, std::stringstream& out)
        {
            std::optional<Sqlite3Rows> rows = ExecuteSql(db, c_TeamStandingsQuery);
            if (!rows.has_value())
            {
                return false;
            }

            out << "\nStandings:\n";
            for (const Sqlite3Row& row : rows.value())
            {
                const std::optional<TeamRecord> teamRecordOpt = TeamRecord::FromRow(row);
                if (!teamRecordOpt.has_value())
                {
                    return false;
                }

                const TeamRecord& teamRecord = teamRecordOpt.value();

                out << teamRecord.Name << " " << teamRecord.Wins << "-" << teamRecord.Losses << "-" << teamRecord.Ties;
                out << " (" << std::setprecision(2) << teamRecord.Percent << ")";
                out << "\n";
            }

            return true;
        }

        // ----------------------------------------------------------------------------
        // Top-level algorithm
        // ----------------------------------------------------------------------------
        bool DoWorkInner(bool insertFailure, std::stringstream& out)
        {
            std::optional<unique_sqlite3> spDB = OpenSqlite3("extest.db");
            if (!spDB.has_value())
            {
                return false;
            }

            if (!ExecuteSql(spDB.value().get(), "PRAGMA foreign_keys = ON;"))
            {
                return false;
            }

            if (!InitDatabase(spDB.value().get()))
            {
                return false;
            }

            if (!AddTeams(spDB.value().get()))
            {
                return false;
            }

            if (!CreateSchedule(spDB.value().get()))
            {
                return false;
            }

            if (!PlaySchedule(spDB.value().get(), insertFailure))
            {
                return false;
            }

            if (!ListTeams(spDB.value().get(), out))
            {
                return false;
            }

            if (!WriteSchedule(spDB.value().get(), out))
            {
                return false;
            }

            if (!WriteResults(spDB.value().get(), out))
            {
                return false;
            }

            if (!WriteStandings(spDB.value().get(), out))
            {
                return false;
            }

            return true;
        }
    } // namespace

    // ----------------------------------------------------------------------------
    // DoWork entry point
    // ----------------------------------------------------------------------------
    AlgorithmOutput DoWork(size_t iterations, bool insertFailure)
    {
        auto start = std::chrono::high_resolution_clock::now();

        std::string output;

        for (size_t i = 0; i < iterations; ++i)
        {
            std::stringstream out;

            if (!DoWorkInner(insertFailure, out))
            {
                out << "The operation failed\n";
            }

            if (output.empty())
            {
                output = out.str();
            }
        }

        std::chrono::milliseconds elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

        return {output, elapsed};
    }
} // namespace ErrorHandlingTest::UseOptionals
