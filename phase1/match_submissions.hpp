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

// ----------------- These are Claude Written (From this point onward till mentioned)---------------------

// Rolling hash for a window of length `len` starting at `start`.
long long compute_hash(const std::vector<int>& seq, int start, int len,
                        long long base, long long mod) {
    long long h = 0;
    for (int i = start; i < start + len; ++i)
        h = (h * base + seq[i] + 1) % mod;   // +1 avoids issues with token 0
    return h;
}

// Verify actual equality once hashes collide (protects against false hits).
bool tokens_equal(const std::vector<int>& a, int ai,
                   const std::vector<int>& b, int bi, int len) {
    for (int k = 0; k < len; ++k)
        if (a[ai + k] != b[bi + k]) return false;
    return true;
}

// Finds non-overlapping exact matches of a fixed length using rolling hashes.
// Returns total matched length added, and records claimed intervals in `t`.
int find_exact_matches_len(const std::vector<int>& s1, const std::vector<int>& s2,
                            int len, IntervalTree& t) {
    long long base = 131, mod = 1'000'000'007;
    long long pow_len = 1;
    for (int i = 0; i < len - 1; ++i) pow_len = (pow_len * base) % mod;

    // Map from hash -> list of start indices in s1 (that aren't yet claimed).
    std::unordered_map<long long, std::vector<int>> hash_index;
    for (int i = 0; i + len <= (int)s1.size(); ++i)
        hash_index[compute_hash(s1, i, len, base, mod)].push_back(i);

    int total = 0;
    long long rolling = compute_hash(s2, 0, len, base, mod);
    for (int j = 0; j + len <= (int)s2.size(); ++j) {
        if (j > 0) {
            long long prev = (rolling - (long long)(s2[j - 1] + 1) * pow_len % mod + mod * mod) % mod;
            rolling = (prev * base + s2[j + len - 1] + 1) % mod;
        }
        auto it = hash_index.find(rolling);
        if (it == hash_index.end()) continue;
        for (int cand : it->second) {
            if (!tokens_equal(s1, cand, s2, j, len)) continue;
            if (t.addInterval(cand, cand + len, j)) { total += len; break; }
        }
    }
    return total;
}


// Extends outward from an anchor exact-match interval, tolerating gaps (insertions/deletions)
// and mismatches while maintaining the 80% subsequence bound. 
// Uses a greedy lookahead to "re-sync" pointers after a gap.
int extend_approx_match(const std::vector<int>& s1, const std::vector<int>& s2,
                         int anchor1, int anchor_end1,
                         int anchor2, int anchor_end2,
                         int& out_start1, int& out_start2, int& out_end1) {
    
    int matched = anchor_end1 - anchor1;
    int unmatched = 0;

    int left1 = anchor1, left2 = anchor2;
    int right1 = anchor_end1, right2 = anchor_end2;

    int best_matched = matched;
    int best_left1 = left1, best_left2 = left2, best_right1 = right1;

    // How far ahead to search for a sync point after a mismatch
    const int max_lookahead = 4; 

    // Loop until we run out of array bounds or mismatch budget
    while (left1 > 0 || left2 > 0 || right1 < (int)s1.size() || right2 < (int)s2.size()) {
        bool advanced = false;

        // 1. Consume "Free" Matches (0 cost to budget)
        if (left1 > 0 && left2 > 0 && s1[left1 - 1] == s2[left2 - 1]) {
            matched++; left1--; left2--; advanced = true;
        }
        if (right1 < (int)s1.size() && right2 < (int)s2.size() && s1[right1] == s2[right2]) {
            matched++; right1++; right2++; advanced = true;
        }

        if (advanced) {
            if (matched > best_matched) {
                best_matched = matched; best_left1 = left1; best_left2 = left2; best_right1 = right1;
            }
            continue; // Keep looping to consume all consecutive exact matches
        }

        // 2. We hit a mismatch. Spend budget to jump gaps (Lookahead)
        // Check left gap
        if (left1 > 0 && left2 > 0) {
            for (int gap = 1; gap <= max_lookahead; ++gap) {
                // Check if s1 has extra tokens
                if (left1 - 1 - gap >= 0 && s1[left1 - 1 - gap] == s2[left2 - 1]) {
                    if (unmatched + gap <= 0.25 * (matched + 1)) {
                        unmatched += gap; left1 -= (gap + 1); left2--; matched++; advanced = true; break;
                    }
                }
                // Check if s2 has extra tokens
                if (left2 - 1 - gap >= 0 && s1[left1 - 1] == s2[left2 - 1 - gap]) {
                    if (unmatched + gap <= 0.25 * (matched + 1)) {
                        unmatched += gap; left1--; left2 -= (gap + 1); matched++; advanced = true; break;
                    }
                }
            }
        }

        // Check right gap
        if (!advanced && right1 < (int)s1.size() && right2 < (int)s2.size()) {
            for (int gap = 1; gap <= max_lookahead; ++gap) {
                // Check if s1 has extra tokens
                if (right1 + gap < (int)s1.size() && s1[right1 + gap] == s2[right2]) {
                    if (unmatched + gap <= 0.25 * (matched + 1)) {
                        unmatched += gap; right1 += (gap + 1); right2++; matched++; advanced = true; break;
                    }
                }
                // Check if s2 has extra tokens
                if (right2 + gap < (int)s2.size() && s1[right1] == s2[right2 + gap]) {
                    if (unmatched + gap <= 0.25 * (matched + 1)) {
                        unmatched += gap; right1++; right2 += (gap + 1); matched++; advanced = true; break;
                    }
                }
            }
        }

        if (advanced) {
            if (matched > best_matched) {
                best_matched = matched; best_left1 = left1; best_left2 = left2; best_right1 = right1;
            }
            continue; 
        }

        // 3. Lookahead failed to find a sync. Spend budget on a pure mismatch/substitution
        if (left1 > 0 && left2 > 0 && unmatched + 1 <= 0.25 * matched) {
            unmatched++; left1--; left2--; advanced = true;
        } else if (right1 < (int)s1.size() && right2 < (int)s2.size() && unmatched + 1 <= 0.25 * matched) {
            unmatched++; right1++; right2++; advanced = true;
        }

        // If we still haven't advanced, we've entirely exhausted our 80% budget
        if (!advanced) break; 

        if (matched > best_matched) {
            best_matched = matched; best_left1 = left1; best_left2 = left2; best_right1 = right1;
        }
    }

    out_start1 = best_left1;
    out_start2 = best_left2;
    out_end1 = best_right1;
    return best_matched;
}

int find_longest_approx_match(const std::vector<int>& sub1, const std::vector<int>& sub2, 
                              const IntervalTree& t, int& index_first, int& index_second) {
    int absolute_longest = 0;

    for (const auto& interval : t.intervals) {
        // Skip short anchors to save time (e.g., only expand from exact matches of length >= 10)
        if (interval.size < 10) continue; 

        int start1 = 0, start2 = 0, end1 = 0;
        int approx_len = extend_approx_match(sub1, sub2, 
                                             interval.start, interval.end, 
                                             interval.indSec, interval.indSec + interval.size, 
                                             start1, start2, end1);
        
        if (approx_len > absolute_longest) {
            absolute_longest = approx_len;
            index_first = start1;
            index_second = start2;
        }
    }

    return absolute_longest;
}

// Till here it is claude written code.

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

    // 1. EXACT SHORT MATCHES (10-20 tokens)
    // Using Claude's rolling hash approach from longest to shortest(efficient but less accurate)
    // for (int len = 25; len >= 10; --len) {
    //     totalMatchlength += find_exact_matches_len(submission1, submission2, len, t);
    // }

    exact_short_match(submission1, submission2, 10, 25, t);
    for(auto& it:t.intervals){
        totalMatchlength += it.size;
    }

    // 2. LONGEST APPROXIMATE MATCH (>= 30 tokens)
    // This Claude approach is efficient but less accurate
    // longestmatch = find_longest_approx_match(submission1, submission2, t, index_first, index_second);
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
