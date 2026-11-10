//
// Test suite for HW5 Scheduling 
//

#include <gtest/gtest.h>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "schedwork.h"

using namespace std;

std::ostream& operator<<(std::ostream& ostr, const AvailabilityMatrix& avail)
{
	for(const auto& vec : avail){
		for(const auto& free : vec){
			ostr << free << " ";
		}
		ostr << endl;
	}
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const DailySchedule& sched)
{
	for(const auto& vec : sched){
		for(const auto& nid : vec){
			ostr << nid << " ";
		}
		ostr << endl;
	}
	return ostr;
}

testing::AssertionResult runAndVerifySolution(
	const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
	bool expectSolution)
{
	bool result = true;
	ostringstream err;

	DailySchedule sched;
	bool solutionExists = schedule(avail, dailyNeed, maxShifts, sched);

	if(expectSolution != solutionExists)
	{
		result = false;
		err << "For availability:\n" << avail << endl;
		err << "Error in schedule:\n" << sched << endl;
		err  << "Mismatch with boolean return result (expected: " << expectSolution << " / actual: " << solutionExists << ")";
		return testing::AssertionFailure() << err.str() << endl;
	}	
	if(expectSolution == false && solutionExists == false)
	{
		return testing::AssertionSuccess();
	}
	else   // Solution exists and is expected, so let's check it
	{
		if(avail.size() != sched.size())
		{
			result = false;
			err << "\nFor availability:\n" << avail << endl;
			err << "Error in schedule:\n" << sched << endl;
			err  << "Number of days mismatched for availability and schedule : " << avail.size() << " / " << sched.size();
			return testing::AssertionFailure() << err.str() << endl;
		}
		else if(avail.size() == 0)
		{
			return testing::AssertionSuccess();
		}


		// Get number of nurses
		std::vector<unsigned> numDays(avail[0].size(), 0U);

		for(const auto& dayVec : sched)
		{
			for(auto nid : dayVec)
			{
				if(nid >= numDays.size())
				{
					result = false;
					err << "\nFor availability:\n" << avail << endl;
					err << "Error in schedule:\n" << sched << endl;
					err << "Invalid nurse ID: " << nid << " in your schedule is invalid.";
				}

				numDays[nid]++;
			}
		}
		for(unsigned k = 0; k < numDays.size(); k++)
		{
			if(numDays[k] > maxShifts)
			{
				result = false;
				err << "\nFor availability:\n" << avail << endl;
				err << "\nError in schedule:\n" << sched << endl;
				err << "\nNurse " << k << " is scheduled for " << numDays[k] 
					<< " shifts (when only " << maxShifts << " are allowed).";
			}
		}
		if(result == false)
		{
			return testing::AssertionFailure() << err.str() << endl;			
		}
		return testing::AssertionSuccess();

	}
}

/********************************************
 *    Scheduling Tests
 ********************************************/
TEST(Schedule, SimpleSol1)
{
    size_t dailyNeed = 1;
    size_t maxShifts = 3;
	bool expectedResult = true;
    AvailabilityMatrix avail = { 
        {1},
		{1},
        {1}
    };

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, SimpleNoSolMaxShifts)
{
    size_t dailyNeed = 1;
    size_t maxShifts = 2;
	bool expectedResult = false;
    AvailabilityMatrix avail = { 
        {1},
		{1},
        {1}
    };

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, SimpleNoSolNoAvail1)
{
    size_t dailyNeed = 1;
    size_t maxShifts = 3;
	bool expectedResult = false;
    AvailabilityMatrix avail = { 
        {0},
		{1},
        {1}
    };

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, SimpleNoSolNoAvail2)
{
    size_t dailyNeed = 1;
    size_t maxShifts = 3;
	bool expectedResult = false;
    AvailabilityMatrix avail = { 
        {1},
		{0},
        {1}
    };

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, SimpleNoSolNoAvail3)
{
    size_t dailyNeed = 1;
    size_t maxShifts = 3;
	bool expectedResult = false;
    AvailabilityMatrix avail = { 
        {1},
		{1},
        {0}
    };

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, Nominal1)
{
    size_t dailyNeed = 2;
    size_t maxShifts = 2;
	bool expectedResult = true;
    AvailabilityMatrix avail = { 
        {1, 1, 1, 1},
        {1, 0, 1, 0},
        {1, 1, 0, 1},
        {1, 0, 0, 1}
    };

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, Nominal1NoSol1)
{
    size_t dailyNeed = 3;
    size_t maxShifts = 2;
	bool expectedResult = false;
    AvailabilityMatrix avail = { 
        {1, 1, 1, 1},
        {1, 0, 1, 0},
        {1, 1, 0, 1},
        {1, 0, 0, 1}
    };

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, Nominal1NoSol2)
{
    size_t dailyNeed = 2;
    size_t maxShifts = 1;
	bool expectedResult = false;
    AvailabilityMatrix avail = { 
        {1, 1, 1, 1},
        {1, 0, 1, 0},
        {1, 1, 0, 1},
        {1, 0, 0, 1}
    };

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, Nominal2)
{
    size_t dailyNeed = 3;
    size_t maxShifts = 3;
	bool expectedResult = true;
    AvailabilityMatrix avail = { 
        {0, 1, 0, 1, 1, 0, 1},
        {0, 1, 1, 0, 1, 0, 0},
        {1, 0, 0, 1, 1, 0, 0},
        {1, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 1, 0},
        {1, 1, 1, 0, 1, 1, 1},
        {1, 0, 0, 1, 1, 1, 0}
    };

	// 0: 1 3 6 
	// 1: 1 2 4 
	// 2: 0 3 4 
	// 3: 0 1 6
	// 4: 0 2 5
	// 5: 2 5 6
	// 6: 3 4 5 


	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, Instructor1)
{
    size_t dailyNeed = 4;
    size_t maxShifts = 4;
	bool expectedResult = true;
    AvailabilityMatrix avail = { 
        {1, 0, 1, 0, 1, 1, 0, 1}, // 2, 4, 5, 7
        {1, 0, 1, 0, 0, 1, 1, 0}, // 0, 2, 5, 6 
        {1, 1, 0, 0, 0, 0, 1, 1}, // 0, 1, 6, 7
        {0, 1, 0, 1, 1, 0, 1, 0}, // 1, 3, 4, 6
        {0, 1, 1, 0, 1, 0, 0, 1}, // 1, 2, 4, 7
        {1, 1, 0, 1, 1, 1, 0, 1}, // 0, 2, 3, 5
        {1, 1, 0, 1, 1, 0, 0, 1}  // 0, 3, 4, 7
    };

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}

TEST(Schedule, Instructor2NoSol)
{
    size_t dailyNeed = 3;
    size_t maxShifts = 3;
	bool expectedResult = false;
    AvailabilityMatrix avail = { 
        {1, 1, 1, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1}
    };
	// 0: 3 4 5 
	// 1: 0 2 5
	// 2: 1 3 6 
	// 3: 0 1 6
	// 4: 1 2 4 
	// 5: 0 3 4 
	// 6: 2 5 6

	testing::AssertionResult result = runAndVerifySolution(
		avail, dailyNeed, maxShifts, expectedResult);
	EXPECT_TRUE(result);
}
