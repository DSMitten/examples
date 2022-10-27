#include "ErrorHandlingTest.h"
#include "LeagueInfo.h"
#include "SqlQueries.h"
#include "Sqlite3Helpers.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <format>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <string_view>

using namespace Sqlite3Helpers::WithExceptions;

namespace ErrorHandlingTest::UseExceptions {
    namespace {

        // ----------------------------------------------------------------------------
        // Database operations
        // ----------------------------------------------------------------------------
        unique_sqlite3 OpenTestDatabase()
        {
            return OpenSqlite3("extest.db");
        }

        void InitDatabase(sqlite3* db)
        {
            ExecuteSql(db, c_InitDatabaseQuery);
        }

        void AddTeam(sqlite3* db, std::string_view city, std::string_view name)
        {
            ExecuteSql(db, MakeInsertTeamQuery(city, name));
        }

        void AddMatch(sqlite3* db, size_t week, std::string_view homeCity, std::string_view awayCity)
        {
            ExecuteSql(db, MakeInsertMatchQuery(week, homeCity, awayCity));
        }

        void AddResult(sqlite3* db, std::string_view matchId, int homeScore, int awayScore)
        {
            ExecuteSql(db, MakeInsertResultQuery(matchId, homeScore, awayScore));
        }

        // ----------------------------------------------------------------------------
        // Populating teams table
        // ----------------------------------------------------------------------------
        void AddTeams(sqlite3* db)
        {
            for (const TeamInfo& teamInfo : c_Teams)
            {
                AddTeam(db, teamInfo.City, teamInfo.Name);
            }
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

        void CreateSchedule(sqlite3* db)
        {
            for (size_t week = 1; week <= c_SeasonLength; week++)
            {
                std::vector<size_t> pickedTeams {};
                while (pickedTeams.size() < c_Teams.size())
                {
                    const size_t homeIndex = PickRandomTeam(pickedTeams); // Pick any team
                    const size_t awayIndex = PickRandomTeam(pickedTeams); // Pick any team other than the home team
                    AddMatch(db, week, c_Teams[homeIndex].City, c_Teams[awayIndex].City);
                }
            }
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

        void PlayWeek(sqlite3* db, size_t week, bool insertFailure)
        {
            std::string sql = MakeGetMatchesQuery(week);

            Sqlite3Rows rows = ExecuteSql(db, sql);

            for (const Sqlite3Row& row : rows)
            {
                int homeScore = SimulateScore();
                int awayScore = SimulateScore();
                AddResult(db, row.at("matchId").value(), homeScore, awayScore);
            }
        }

        void PlaySchedule(sqlite3* db, bool insertFailure)
        {
            for (size_t week = 1; week <= c_SeasonLength; week++)
            {
                PlayWeek(db, week, insertFailure);
            }
        }

        // ----------------------------------------------------------------------------
        // Writing teams
        // ----------------------------------------------------------------------------
        void ListTeams(sqlite3* db, std::stringstream& out)
        {
            Sqlite3Rows rows = ExecuteSql(db, c_ListTeamsQuery);

            out << "\nTeams:\n";
            for (const TeamInfo& teamInfo : rows)
            {
                out << teamInfo.City << " " << teamInfo.Name << "\n";
            }
        }

        // ----------------------------------------------------------------------------
        // Writing schedule
        // ----------------------------------------------------------------------------
        void WriteSchedule(sqlite3* db, std::stringstream& out)
        {
            Sqlite3Rows rows = ExecuteSql(db, c_LeagueScheduleQuery);

            out << "\nSchedule:\n";
            int week = -1;
            for (const Match& match : rows)
            {
                if (week != match.Week)
                {
                    week = match.Week;
                    out << "Week " << week << ":\n";
                }

                out << "    " << match.Away << " at " << match.Home << "\n";
            }
        }

        // ----------------------------------------------------------------------------
        // Writing scores
        // ----------------------------------------------------------------------------
        void WriteResult(std::string_view team1, int team1Score, std::string_view team2, int team2Score, std::stringstream& out)
        {
            out << "    " << team1 << ": " << team1Score << ", " << team2 << ": " << team2Score << "\n";
        }

        void WriteResults(sqlite3* db, std::stringstream& out)
        {
            Sqlite3Rows rows = ExecuteSql(db, c_LeagueResultsQuery);

            out << "\nResults:\n";
            int week = -1;
            for (const Result& result : rows)
            {
                if (week != result.Week)
                {
                    week = result.Week;
                    out << "Week " << week << ":\n";
                }

                WriteResult(result.Away, result.AwayScore, result.Home, result.HomeScore, out);
            }
        }

        // ----------------------------------------------------------------------------
        // Writing team standings
        // ----------------------------------------------------------------------------
        void WriteStandings(sqlite3* db, std::stringstream& out)
        {
            Sqlite3Rows rows = ExecuteSql(db, c_TeamStandingsQuery);

            out << "\nStandings:\n";
            for (const TeamRecord& teamRecord : rows)
            {
                out << teamRecord.Name << " " << teamRecord.Wins << "-" << teamRecord.Losses << "-" << teamRecord.Ties;
                out << " (" << std::setprecision(2) << teamRecord.Percent << ")";
                out << "\n";
            }
        }

        // ----------------------------------------------------------------------------
        // Top-level algorithm
        // ----------------------------------------------------------------------------
        void DoWorkInner(bool insertFailure, std::stringstream& out)
        {
            unique_sqlite3 spDB = OpenSqlite3("extest.db");
            ExecuteSql(spDB.get(), "PRAGMA foreign_keys = ON;");
            InitDatabase(spDB.get());
            AddTeams(spDB.get());
            CreateSchedule(spDB.get());

            PlaySchedule(spDB.get(), insertFailure);

            ListTeams(spDB.get(), out);
            WriteSchedule(spDB.get(), out);
            WriteResults(spDB.get(), out);
            WriteStandings(spDB.get(), out);
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

            try
            {
                DoWorkInner(insertFailure, out);
            }
            catch (const std::system_error& ex)
            {
                out << "error occurred: " << ex.what() << "\n";
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
} // namespace ErrorHandlingTest::UseExceptions
