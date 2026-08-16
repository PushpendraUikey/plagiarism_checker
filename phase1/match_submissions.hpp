#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>

#include<set>
#include<unordered_set>
#include<unordered_map>
#include<algorithm>
#include <cassert>
#include <chrono>
#include <tuple>


/*
Author: Pushpendra Uikey    23b1023
        Akshit Kumar        23b0945
*/
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

/*------------------------------------ Data Struct to store exact matches----------------------------------*/
// To store match intevals

struct Interval {
    int start, end, size, indSec;
    Interval(int s, int e, int indSec) : start(s), end(e), size(e - s + 1), indSec(indSec) {}
};

class IntervalTree {
public:
    std::vector<Interval> intervals;  // Store intervals

    // Add interval if non-overlapping
    bool addInterval(int start, int end, int indSec) {
        Interval newInterval(start, end, indSec);

        // Binary search for the position of the interval
        auto it = std::lower_bound(intervals.begin(), intervals.end(), newInterval, [](const Interval& a, const Interval& b) {
            return a.end < b.start;
        });

        // Check overlap with previous and next intervals
        if ((it != intervals.end() && newInterval.end > it->start) || 
            // (it != intervals.end() && newInterval.start < it->end && newInterval.indSec < it->indSec + it->size) ||
            (it != intervals.begin() && std::prev(it)->end > newInterval.start) 
            // (it != intervals.begin() && newInterval.end > std::prev(it)->start && std::prev(it)->indSec + std::prev(it)->size > newInterval.indSec)
            ) {
            return false;  // Overlap detected
        }

        intervals.insert(it, newInterval);  // Insert maintaining order
        return true;
    }
};


/* ------------------------------- exact short sequences match length; -------------------------------*/
// this is the helper function
bool solve(std::vector<int>& sub1, std::vector<int>& sub2, int i, int j, int len, IntervalTree& t) {
    for (int r = 0; r < len; ++r) {
        if (sub1[i + r] != sub2[j + r]) {
            return false;      // Subsequence did not match
        }
    }
    // If match found, attempt to insert into the interval tree
    return t.addInterval(i, i + len, j);
}

void exact_short_match(std::vector<int>& sub1, std::vector<int>& sub2, 
                        int mini, int maxi, IntervalTree& t) {

    for (int len = maxi; len >= mini; --len) {  // maximum token sizes given priority to check if exact match exists
        int i = 0;

        while (i <= sub1.size() - len) {        // taking care of out-of-bound range
            bool match_found = false;      

            for (int j = 0; j <= sub2.size() - len; ++j) {
                if (solve(sub1, sub2, i, j, len, t)) {  // if successfully inserted
                    i += len-1 ;        // Move to next segment of `sub1`
                    match_found = true;
                    break;              // Move on to next `i` in `sub1`
                }
            }

            if (!match_found) {
                i++;
            }
        }
    }
}


/*------------------------------- Longest approx Match detection -----------------------------*/

int approximateMatch(const std::vector<int>& sub1, const std::vector<int>& sub2, int& startFirst, int& startSec) {
    int n = sub1.size(), m = sub2.size();
    int maxMatch = 0;

    // DP table for storing longest match lengths
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (sub1[i - 1] == sub2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] > maxMatch) {
                    maxMatch = dp[i][j];
                    startFirst = i - maxMatch;
                    startSec = j - maxMatch;
                }
            } else {
                dp[i][j] = 0;  // Reset match
            }
        }
    }

    return maxMatch;
}

bool accomodationPossible(int unmatch, int matched){
    return unmatch <= 0.1*matched;      //this threshold says if in any approx_match if the match is
                                        // 90% above(i.e. unmatched is less 10%) it is potential approx match
}

int longestApproxMatch(IntervalTree& t, int& fir, int& sec) {
    int unmatched = 0;        // Track unmatched tokens between intervals
    int startFirst = 0;       // Start index of max matched subsequence in submission 1
    int startSec = 0;         // Start index of max matched subsequence in submission 2
    int maxMatched = 0;       // Longest approximate matched subsequence found
    int lastEnd = 0;          // Track end of last interval to calculate gaps
    int matched = 0;          // Track length of current matched subsequence

    for (const auto& it : t.intervals) {
        int start = it.start;
        
        // Calculate matched length and new unmatched gap
        int newGap = start - lastEnd;
        unmatched += newGap;  // Add the new gap to unmatched count
        matched += it.size;   // Add interval size to current match count
        
        // Check if we can accommodate the current unmatched count in this sequence
        if (accomodationPossible(unmatched, matched)) {
            // Update max match if the current matched is longest so far
            if (matched > maxMatched) {
                maxMatched = matched;
                fir = startFirst;
                sec = startSec;
            }
        } else {
            // Reset matching sequence due to large unmatched gap
            startFirst = start;    // New start point in subm 1
            startSec = it.indSec;  // New start point in subm 2
            matched = 0;           // Restart match count with current interval(it can be start with 'it.size' including current window size to match)
            unmatched = 0;         // Reset unmatched as we start new sequence
        }
        
        lastEnd = it.end;  // Update lastEnd to current interval's end
    }
    
    return maxMatched;  // Return the longest approximate matched subsequence
}


std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {

    bool plagiarised=false;         // if files are plagiarised or not(intial assumpt not)
    int totalMatchlength=0;         // total exact token mathches
    int longestmatch=0;             // longest approximate match
    int index_first=0;              // starting index of longest approximate match in first file
    int index_second=0;             // starting index of longest approximate match in second file
    

    IntervalTree t;
    exact_short_match(submission1, submission2, 10, 25, t);

    for(auto& it:t.intervals){
        totalMatchlength += it.size;
    }

    // longestmatch = longestApproxMatch(t, index_first, index_second);
    longestmatch = approximateMatch(submission1, submission2, index_first, index_second);

    if(longestmatch >= 0.3*submission2.size() || totalMatchlength>= 0.4*submission2.size())
        plagiarised = true;

    std::array<int, 5> result = {plagiarised, totalMatchlength, longestmatch, index_first, index_second};
    return result;
}


// class IntervalTree {
// public:
//     std::set<Interval> intervals;   // to store intervals
// public:
//     // Function to check overlap and insert if no overlap
//     bool addInterval(int start, int end, int indSec) {
//         Interval newInterval(start, end, indSec);

//         // Checking for overlap with the next interval
//         /*Attribution: https://www.geeksforgeeks.org/lower_bound-in-cpp/ */
//         auto it = intervals.lower_bound(newInterval);

//         if (it != intervals.end()) {
//             // newinterval should not overlap with the next interval, in both submission file(preventing overcounting)
//             if (newInterval.end > it->start || 
//                 (newInterval.start < it->end && newInterval.indSec < it->indSec + it->size)) {
//                 return false;       // Overlap found with next, do not insert
//             }
//         }

//         // Checking for overlap with the previous interval
//         if (it != intervals.begin()) {
//             auto prev = std::prev(it);
//             // newinterval shouldn't overlap in both submission file 
//             if (prev->end > newInterval.start || 
//                 (newInterval.end > prev->start && prev->indSec + prev->size > newInterval.indSec)) {
//                 return false; // Overlap found with previous, do not insert
//             }
//         }

//         // No overlap found; insert the new interval
//         intervals.insert(newInterval);
//         return true;
//     }
// };
