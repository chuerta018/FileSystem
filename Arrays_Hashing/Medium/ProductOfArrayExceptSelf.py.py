"""
Given an integer array nums, return an array answer such that answer[i] is equal to the product of all the elements of nums except nums[i].

The product of any prefix or suffix of nums is guaranteed to fit in a 32-bit integer.

You must write an algorithm that runs in O(n) time and without using the division operation.

"""
class Solution(object):
    def productExceptSelf(self, nums):
        """
        :type nums: List[int]
        :rtype: List[int]
        """
        answer = [1] * len(nums)

    # Left pass: fill in the prefix product
        for i in range(1, len(nums)):
            answer[i] = nums[i - 1] * answer[i - 1]

    # R to hold the running product of elements to the right
        R = 1
    # Right pass: update the answer array with the product of elements to the right
        for i in reversed(range(len(nums))):
            answer[i] = answer[i] * R
            R *= nums[i]

        return answer
        
