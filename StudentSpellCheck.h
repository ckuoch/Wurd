#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"
#include <iostream>
#include <cctype>
#include <string>
#include <vector>

const int ALPHABET = 27;
const char ALPHA[27] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\'' };
class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() { }
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct TrieNode
	{
		TrieNode* children[ALPHABET]; //create trie with a bool to indicate end of trie and a vector of node pointers
		bool end;

		TrieNode()  // construct trie with false end and nullptrs
		{
			end = false;
			for (int i = 0; i < ALPHABET; i++)
				children[i] = nullptr;
		}
		~TrieNode() {
			TrieNode* node;
			for (int i = 0; i < ALPHABET; i++) {
				node = children[i];
				if (node != nullptr) {
					delete node;
				}
			}
		}
		// Helper Functions for Trie
		void insert(TrieNode* root, std::string value)
		{
			TrieNode* word = root; //create a crawler pointer to move through trie nodes
			for (int i = 0; i < value.length(); i++) //for length of value string
			{
				value[i] = tolower(value[i]); //set every character to lower if not already
				int index = value[i] - 'a'; //convert character to index value
				if (value[i] == '\'')
					index = 26;
				if (word->children[index] == nullptr)
					word->children[index] = new TrieNode; //create a new trienode at that index indicating that letter if it doesnt already exist

				word = word->children[index]; //move trie to next node
			}
			// last node must be leaf so 
			word->end = true; //once you reach the last letter or character node, set its end to false
		}

		bool search(TrieNode* root, std::string word)
		{
			TrieNode* search = root; //create a crawler pointer to move through trie nodes
			for (int i = 0; i < word.length(); i++)
			{
				word[i] = tolower(word[i]); //set every character to lower if not already
				int index = word[i]; //convert character to index value
				if (word[i] == '\'')
					index = 26;
				else
					index = word[i] - 'a';
				// if any charcter index of the search word is not found, or is null in the trie, return false meaning that the word is not part of the dictionary
				if (search->children[index] == nullptr)
				{
					return false; 
				}
				//otherwise continue to search the trie
				search = search->children[index];
			}
			// return (if the last visited node is a leaf and not a nullptr)
			return (search != nullptr && search->end);
		}
	};

	TrieNode* m_root = new TrieNode;
};

#endif  // STUDENTSPELLCHECK_H_
