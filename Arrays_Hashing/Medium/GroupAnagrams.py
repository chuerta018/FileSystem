"""
Given an array of strings strs, group the anagrams together. You can return the answer in any order.

An Anagram is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.

 

Example 1:

Input: strs = ["eat","tea","tan","ate","nat","bat"]
Output: [["bat"],["nat","tan"],["ate","eat","tea"]]
Example 2:

Input: strs = [""]
Output: [[""]]
Example 3:

Input: strs = ["a"]
Output: [["a"]]
"""



class Solution(object):
    def groupAnagrams(self, strs):
        """
        :type strs: List[str]
        :rtype: List[List[str]]
        """
        # A map declaration
        ourMap = {}
        
        #Iterate thru the invididual strings of strs provided 
        for s in strs:
           #Sort the strings, bc matching sorted strings is anagram
           #Store in a tuple, immutable list as sorted list
           #sorted() returns a list of the sorted chars 
            sorted_str = tuple(sorted(s))
            
            # check if an existing tuple exist
            if sorted_str not in ourMap:
                #no?, create a list with the unsorted|original string, s 
                ourMap[sorted_str] = [s]  
            else:
                # yes, add to current matching tuple's list
                ourMap[sorted_str].append(s)  

       #return a list of our map's values holding the original strings
        return list(ourMap.values())



#=====================================  Great answer same logic ==================================#

class Solution:
    def groupAnagrams(self, strs):
      #defaultdict
      # When the defaultdict object is created, it is given the list type as the default factory function.
      # This means that every time a new key is accessed that isn't present in the dictionary, defaultdict will automatically initialize it with an empty list [].
        anagram_map = defaultdict(list)
        
        for word in strs:
        # sorted_word is empty string and joins the sortedword list
        # join concatenates iterable items into a string aka the sorted chars list from sorted()
            sorted_word = ''.join(sorted(word))
            anagram_map[sorted_word].append(word)
        
        return list(anagram_map.values())
