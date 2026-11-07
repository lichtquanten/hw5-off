//
// Test suite for HW5 Wordle 
//

#include "wordle.h"
#include "dict-eng.h"
#include <gtest/gtest.h>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

testing::AssertionResult compareResults(
	const std::set<std::string>& expected,
	std::set<std::string> actual)
{
	bool result = true;
	ostringstream err;
	for(auto s : expected)
	{
		if(actual.find(s) == actual.end())
		{
			result = false;
			err  << "\nDid not find: " << s << " in actual results.";
		}
		else
		{
			actual.erase(s);
		}
	}
	if(actual.size() > 0)
	{
		result = false;
		for(auto s : actual)
		{
			err << "\nExtra string " << s << " found in actual results";
		}
	}
	if(result == false)
	{
		return testing::AssertionFailure() << err.str() << endl;
	}
	else 
	{
		return testing::AssertionSuccess();
	}
}


const std::set<std::string>& dict = readDictWords("dict-eng.txt");    

/********************************************
 *    Wordle Tests
 ********************************************/
TEST(Wordle, Empty)
{
	std::set<std::string> expected = {
		
	};
    std::set<std::string> actual = wordle("", "", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, NoSolutions)
{
	std::set<std::string> expected = {
		
	};
    std::set<std::string> actual = wordle("--", "xy", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, OneLetterUnconstrained)
{
	std::set<std::string> expected = {
		
	};
	for(char c = 'a'; c <= 'z'; c++)
	{
		string temp;
		temp += c;
		expected.insert(temp);
	}

    std::set<std::string> actual = wordle("-", "", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, OneLetterConstrained)
{
	std::set<std::string> expected = {
		"i"
	};

    std::set<std::string> actual = wordle("-", "i", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, TwoLetterUnconstrained)
{
	std::set<std::string> expected = {};

	for(char c = 'a'; c <= 'z'; c++)
	{
		for(char d = 'a'; d <= 'z'; d++) 
		{
			string temp;
			temp += c;
			temp += d;
			if(dict.find(temp) != dict.end())
			{
				expected.insert(temp);
			}
		}
	}

    std::set<std::string> actual = wordle("--", "", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, TwoLetterConstrained)
{
	std::set<std::string> expected = {
		"ad",
		"ah",
		"am",
		"an",
		"as",
		"at",
		"ax",
		"ay",
		"fa",
		"ha",
		"la",
		"ma",
		"pa"
	};

    std::set<std::string> actual = wordle("--", "a", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, ThreeLetterFull)
{
	std::set<std::string> expected = {
		"her"
	};

    std::set<std::string> actual = wordle("her", "", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, OneOption)
{
	std::set<std::string> expected = {
		"her"
	};

    std::set<std::string> actual = wordle("he-", "r", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, Nominal1)
{
	std::set<std::string> expected = {
		"corrosion",
		"recursion"
	};
    std::set<std::string> actual = wordle("-----sion", "rcr", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, Nominal2)
{
	std::set<std::string> expected = {
		"patted",
		"patent",
		"patter"
	};
    std::set<std::string> actual = wordle("pa----", "tt", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}



TEST(Wordle, Nominal3)
{
	std::set<std::string> expected = {
		"boars",
		"boast",
		"boats",
		"coals",
		"coast",
		"coats",
		"foals",
		"foams",
		"goads",
		"goals",
		"goats",
		"loads",
		"loafs",
		"loans",
		"moans",
		"moats",
		"roads",
		"roams",
		"roans",
		"roars",
		"roast",
		"soaks",
		"soaps",
		"soapy",
		"soars",
		"toads",
		"toast"
	};
    std::set<std::string> actual = wordle("-oa--", "s", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, Long)
{
	std::set<std::string> expected = {
		"compiler",
		"computer",
		"composer"
	};
    std::set<std::string> actual = wordle("-o-----r", "cmpe", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}


TEST(Wordle, Instructor1)
{
	std::set<std::string> expected = {
		"lathe",
		"latte",
		"table"
	};
    std::set<std::string> actual = wordle("-a--e", "lt", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, Instructor2)
{
	std::set<std::string> expected = {
		"programmes",
		"programmer",
		"programmed"
	};
    std::set<std::string> actual = wordle("p--g-a--e-", "mmrro", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}

TEST(Wordle, Instructor3)
{
	std::set<std::string> expected = {
		"telecommunications"
	};
    std::set<std::string> actual = wordle("teleco--uni-----ns", "mmcatio", dict);
	testing::AssertionResult result = compareResults(expected, actual);
	EXPECT_TRUE(result);
}
