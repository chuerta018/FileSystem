
"""
Given an integer array nums and an integer k, return the k most frequent elements. You may return the answer in any order.

 

Example 1:

Input: nums = [1,1,1,2,2,3], k = 2
Output: [1,2]
Example 2:

Input: nums = [1], k = 1
Output: [1]
 
"""


class Solution(object):
    def topKFrequent(self, nums, k):
        """
        :type nums: List[int]
        :type k: int
        :rtype: List[int]
        """
        freq_map = {}
        for num in nums:
            if num in freq_map:
                freq_map[num] += 1
            else:
                freq_map[num] = 1


    # items() provides a way to pair the key and value as num|freq (as stored)
    # heap is A list of tuples with the pair values retrieve from items
    # Using negative frequency to simulate a max-heap using Python's min-heap
    # Python is AUTO a min Heap
        heap = [(-freq, num) for num, freq in freq_map.items()]
    #heapq python module for heap algo
    #heapify() transforms a list or collections of items into a acceptable heap
        heapq.heapify(heap)

    # Extract the k elements with the highest frequency emplace into a list
    # pops the heap for k times
        top_k = [heapq.heappop(heap)[1] for _ in range(k)]

        return top_k
