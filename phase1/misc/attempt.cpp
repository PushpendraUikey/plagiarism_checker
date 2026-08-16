

    // totalMatchlength = find_exact_short_matches_hashing(submission1, submission2, 8, 30);
    // totalMatchlength = find_exact_short_matches_sliding_window(submission1, submission2, 8, 30);
    // longestmatch = LCS(submission1, submission2);

    // longestmatch detection

/*
    if(longestmatch <= 40) {    
        index_first = 0;
        index_second = 0;
        longestmatch = 0;
    }
*/

/*-----------------------------------------------------------------------------------------------*/

/*
int longestApproxMatch(IntervalTree& t, int&fir, int& sec){
    int unmatched=0;        // to keep track of unmatched tokens
    int startfirst=0;       // to keep track of starting index of maximum matched subsequence in subm 1
    int startSec = 0;       // to keep track of starting index of maximum matched subsequence in subm 2
    int maxMatched=0;       // to store maximum approximate matched subsequence
    int lastend = 0;        // to store last interval's end point
    int matched = 0;        // to store approx match

    for(auto& it: t.intervals){
        int start = it.start;
        matched += it.size;
        if(accomodationPossible(unmatched+(start-lastend), matched)){
            unmatched += (start-lastend);
            // maxMatched = std::max(maxMatched, matched);
            if(matched > maxMatched){
                maxMatched = matched;
                fir = startfirst;
                sec = startSec;
            }
        }else{
            startfirst = start;
            startSec = it.indSec;
            matched = 0;
            unmatched = 0;
        }
        lastend = it.end;
    }
    return maxMatched;  // maximum approx matched value to return
}
*/

/*-------------------------------------------------------------------------------------------------*/
/*
    bool addInterval(int start, int end, int indSec) {
    Interval newInterval(start, end, indSec);

    // Look for overlap with the next interval
    auto it = intervals.lower_bound(newInterval);

    // Check for overlap with the next interval
    if (it != intervals.end() && ((newInterval.end > it->start) || 
    (newInterval.indSec+newInterval.size > it->indSec))) {   // second part is for interval collision in second submiss
        return false; // Overlap found, do not insert
    }

    // Check for overlap with the previous interval
    if (it != intervals.begin()) {
        auto prev = std::prev(it);
        if (prev->end > newInterval.start || (prev->indSec+prev->size > newInterval.indSec)) {
            return false; // Overlap found, do not insert
        }
    }
    // No overlap found; insert the new interval
    intervals.insert(newInterval);
    return true;
}
*/
/*-----------------------------------------------------------------------------------------------*/

/*
int findSecondindex(std::vector<int>& sub1, std::vector<int>& sub2,const int startfir, int len){
    int i=0;

    while(i<sub2.size()-len){
        int j=0;
        bool found=true;
        for(j=startfir; j<startfir+len; ++j){
            if(sub1[j] != sub2[i]){
                found = false;
                break;
            }
            i++;
        }
        if(found){
            return (i-len);
        }
        i++;
    }
    return 0;
}
*/

/*----------------------------LCS Algorithm----------------------------------------*/

/*
// Longest Common Subsequence(LCS): O(n*m), it is done using dp
// detecting global similarities when token order matters but gaps are allowed
int LCS(std::vector<int>& seq1, std::vector<int>& seq2) {
    int n = seq1.size(), m = seq2.size();
    std::vector<std::pair<int, int>> unmatched_ind;

    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (seq1[i - 1] == seq2[j - 1])
                dp[i][j] = dp[i - 1][j - 1] + 1;
            else{
                unmatched_ind.push_back({i, j});
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    return dp[n][m];
}
*/

/*-----------------------------------------------------------------------------------------*/
/*  // Trying to find longest approx match by doing process on guessed lenght;
    //// longest match detection
    int s=0; 
    int e = submission1.size()-1;
    int m;

    while(s<e){
        m = s + (e-s)/2;
        if(longestApproxMatch(submission1, submission2, m, index_first, index_second)){
            longestmatch = m;
            s = m+1;
        }else{
            e = m-1;
        }
        m = s+(e-s)/2;
    }

*/

//----------------------------- Silly approach to detect possible match------------------------------
/*
bool checkMatchPossible(std::vector<int> &sub1, std::vector<int> &sub2, int starti, int startj, int len) {
    int match_count1 = 0;
    int match_count2 = 0;
    int i = starti;
    int j = startj;

    while (i < len && j < (5 * len / 4) && j < sub2.size()) {
        if (sub1[i] == sub2[j]) {
            i++;
            j++;
            match_count1++;
        } else {
            j++;
        }
    }
    i=starti;
    j=startj;
    while(j<len && i < (5 * len / 4) && i<sub1.size()){
        if(sub2[j] == sub1[i]){
            i++;
            j++;
            match_count2++;
        }else{
            i++;
        }
    }
    return std::max(match_count1,match_count2) >= len;
}
*/

//---------------------------------------- Silly approach to detect longest approax match------------------
/*
bool longestApproxMatch(std::vector<int> &sub1, std::vector<int> &sub2, int len, int &indFir, int &indSec) {
    int n = sub1.size();
    int m = sub2.size();
    if (m < len) return false; // longest match not possible if sub2 is too small

    for (int i = 0; i < n / len; i++) {
        for (int j = i; j < len + i; j++) { // for sub1 window
            for (int k = 0; k < m - len; ++k) { // for sub2 window
                if (checkMatchPossible(sub1, sub2, j, k, len)) {
                    indFir = i;
                    indSec = j;
                    return true;
                }
            }
        }
    }
    return false;
}
*/


// -------------------------------silly approach to find exact short match----------------------------
/*
void exact_short_match(std::vector<int>&sub1, std::vector<int>&sub2, int mini, int maxi, IntervalTree& t){
    int matched_len = 0;

    for(int len=maxi; len>=mini; --len){
        int i=0; int j=0;
        while(i<sub1.size()-len){
            while(j<sub2.size()-len){
                bool solved = solve(sub1, sub2, i, j, len, t);
                if(solved){
                    i = i+len-1;  //// try to match next segment
                    j=0;
                }else{
                    j++;
                }
            }
            i++;
        }
    }
}
*/


//------------------------------- kgram approach to find exact matches---------------------------
/*
bool kgram_matching(std::vector<int>& seq1, std::vector<int>& seq2, int k) {
    std::unordered_set<string> grams;
    for (int i = 0; i <= seq1.size() - k; ++i)
        grams.insert(to_string(seq1[i]) + "..." + to_string(seq1[i + k - 1]));
    for (int j = 0; j <= seq2.size() - k; ++j) {
        string gram = to_string(seq2[j]) + "..." + to_string(seq2[j + k - 1]);
        if (grams.find(gram) != grams.end()) return true; // Match found
    }
    return false;
}
*/


//-------------------------------------- Rabin-Karp for short exact match detection----------------------

//sliding window with Hashing(Rabin-Karp)
/*
int rabin_karp(const std::vector<int>& seq1, const std::vector<int>& seq2, int len) {
    if (seq1.size() < len || seq2.size() < len) return -1;
    // Compute hash for first 'len' elements in both sequences
    int hash1 = 0, hash2 = 0, base = 31, mod = 1e9 + 7;
    int power = 1;
    for (int i = 0; i < len; i++) {
        hash1 = (hash1 * base + seq1[i]) % mod;
        hash2 = (hash2 * base + seq2[i]) % mod;
        if (i != 0) power = (power * base) % mod;
    }
    if (hash1 == hash2) return 0; // Match found at start
    // Slide the window
    for (int i = len; i < seq1.size(); i++) {
        hash1 = (hash1 - seq1[i - len] * power % mod + mod) % mod;
        hash1 = (hash1 * base + seq1[i]) % mod;
        // Check in seq2 similarly, or precompute a hash map of seq2's hashes
    }
    return -1;
}
*/



// ---------------------------- Hashing approach for exact short match, but it has a bug--------------------

/*
int hash_sequence(std::vector<int>& tokens, int start, int len) {
    long long int hash = 0, base = 31, mod = 1e9 + 7;
    for (int i = start; i < start + len; ++i) {
        hash = (hash * base + tokens[i]) % mod;
    }
    hash = (int)hash;
    return hash;
}

int find_exact_short_matches_hashing(std::vector<int>& tokens1, std::vector<int>& tokens2, 
            int min_len, int max_len) {
    
    int total_match_length = 0;
    for (int len = min_len; len <= max_len; ++len) {
        std::unordered_map<int, int> seen_hashes;
        // Store hashes of all sequences of length len from tokens1
        for (int i = 0; i <= tokens1.size() - len; ++i) {
            seen_hashes[(hash_sequence(tokens1, i, len))]++;
        }
        // Check hashes in tokens2
        for (int j = 0; j <= tokens2.size() - len; ++j) {
            int hash = hash_sequence(tokens2, j, len);
            if (seen_hashes[hash]>0) {
                total_match_length += len;
                seen_hashes[hash]--;
                break;
            }
        }
        seen_hashes.clear();
    }
    return total_match_length;
}
*/



// Longest Common Substring detection: O(n*m)
//--------------------------------- for exact matching of longest common substr----------------------
/*
int LCSstr(std::vector<int>& seq1, std::vector<int>& seq2) {
    int n = seq1.size(), m = seq2.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    int max_len = 0;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (seq1[i - 1] == seq2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                max_len = std::max(max_len, dp[i][j]);
            }
        }
    }
    return max_len;
}
*/