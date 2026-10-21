// 프로그래머스 42576 완주하지 못한 선수 (Lv.1, 해시)
// https://school.programmers.co.kr/learn/courses/30/lessons/42576
// 결과: 정확성 58.3 + 효율성 41.7 = 100.0 / 100.0 (2026-10-22)

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

using namespace std;

string solution(vector<string> participant, vector<string> completion) {
    string answer = "";

    unordered_map<string, int> participantMap;

    for (string& p : participant)
    {
        participantMap[p] += 1;
    }

    for (string& c : completion)
    {
        participantMap[c] -= 1;
    }

    for (auto& m : participantMap)
    {
        if (m.second == 1)
            answer = m.first;
    }

    return answer;
}
