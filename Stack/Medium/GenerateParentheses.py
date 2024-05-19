"""
Given n pairs of parentheses, write a function to generate all combinations of well-formed parentheses.

 

Example 1:

Input: n = 3
Output: ["((()))","(()())","(())()","()(())","()()()"]
Example 2:

Input: n = 1
Output: ["()"]
"""

class Solution(object):
    def generateParenthesis(self, n):
        """
        :type n: int
        :rtype: List[str]
        """
        def backtrack(current_string, open_count, close_count):
            #Base case for a valid string. n=3 we need atleast 3 closing and 3 opening paranthesis 
            if len(current_string) == 2 * n:
                result.append(current_string)
                return
            if open_count < n:
                #1st base case: addes a ( and increase open count.  continues recurision call 
                backtrack(current_string + "(", open_count + 1, close_count)
                #first recusrion result would be ((())), then backtrack via stack frame pters
                #created by each backtrack rescursion call 
            if close_count < open_count:
                backtrack(current_string + ")", open_count, close_count + 1)
               

        result = []
        backtrack("", 0, 0)

        return result
