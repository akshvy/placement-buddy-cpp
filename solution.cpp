#include <iostream>
#include <vector>

using namespace std;

// Two Sum - Brute Force
vector<int> twoSum(vector<int>& nums, int target) {
    for (size_t i = 0; i < nums.size(); i++) {
        for (size_t j = i + 1; j < nums.size(); j++) {
            if (nums[i] + nums[j] == target) {
                return {(int)i, (int)j};
            }
        }
    }
    return {};
}

int main() {
    vector<int> nums = {2, 7, 11, 15};
    int target = 9;
    vector<int> result = twoSum(nums, target);
    if (!result.empty()) {
        cout << "Indices: " << result[0] << ", " << result[1] << endl;
    }
    return 0;
}
