"""
Given an unsorted array of integers nums, return the length of the longest consecutive elements sequence.

You must write an algorithm that runs in O(n) time.

 

Example 1:

Input: nums = [100,4,200,1,3,2]
Output: 4
Explanation: The longest consecutive elements sequence is [1, 2, 3, 4]. Therefore its length is 4.
"""
class Solution(object):
    def longestConsecutive(self, nums):
        """
        :type nums: List[int]
        :rtype: int
        """
        #create a set of nums, removing dupelicates 
        #dupes does not count for an addtional length
        numSet = set(nums)
        # keep track of largestLength 
        largestLen = 0
        
        #iterate thru set of unique nums
        for n in numSet:
            # if number, n, minus one is present, then it is not the start of a sequnce skip
            if (n -1) not in numSet:
                #keep a second variabe length to keep sublenghts at a time
                length = 0
                #recall the numset has all unique elements of nums,
                #we check if the consecutive element is present
                while (n + length) in numSet:
                    length += 1
                #compare the sub-squence to max length. return largest
                largestLen = max( largestLen, length)    
        
        return largestLen
