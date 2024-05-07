"""
Given an array of integers nums and an integer target, return indices of the two numbers such that they add up to target.

You may assume that each input would have exactly one solution, and you may not use the same element twice.

You can return the answer in any order.

Example 1:

Input: nums = [2,7,11,15], target = 9
Output: [0,1]
Explanation: Because nums[0] + nums[1] == 9, we return [0, 1].
Example 2:

Input: nums = [3,2,4], target = 6
Output: [1,2]
Example 3:

Input: nums = [3,3], target = 6
Output: [0,1]

"""

class Solution(object):
    def twoSum(self, nums, target):
        """
        :type nums: List[int]
        :type target: int
        :rtype: List[int]
        """
        #create a map to hold number and index of each object in nums
        ourMap  = {}

        #enumerate returns a pair, the index and corresponding item from an iterable
        #This case index, num from iterable list nums
        for i, num in enumerate(nums):
            #Get the complimentary value 
            comp = target - num
            #If comp value is in our map, return the comp's value index and the current num 
            #This is the two numbers that add up to the target value
            if comp in ourMap:
                return  [ourMap[comp],i]
            else: 
            #if not, add to our num item into our map as the comp was not found.
                ourMap[num] = i
