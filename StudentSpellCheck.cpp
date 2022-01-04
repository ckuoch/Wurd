#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
	delete m_root; //call destroy
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	ifstream infile(dictionaryFile);
	if (!infile) // if file doesnt load
		return false; 
	else
	{
		delete m_root;
		m_root = new TrieNode;
		string s;
		while (getline(infile, s)) // read in document
		{
			for (int i =0; i < s.size(); i++)
			{
				if (!isalpha(s[i]) && s[i] != '\'')
				{
					s.erase(i);
				}
			}
			m_root->insert(m_root, s); // call insert helper function to insert the word
		}
		return true;
	}
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	suggestions.clear();
	if (m_root->search(m_root,word))
		return true;
	else
	{
		// for word
		for (int i = 0; i < word.length(); i++)
		{
			char og = word[i];
			// for each position, change word to new word with different letter of the alphabet
			for (int k = 0; k < ALPHABET; k++)
			{
				word[i] = ALPHA[k];
				//for every new word, check if that word is in the trie
				// if the word is in the trie, push back a new suggestion
				if (m_root->search(m_root, word))
				{
					suggestions.push_back(word);
				}
			}
			// set the character back to its original
			word[i] = og;
		}
		return false;
	}
	
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	// Separate line by words 
	string word = ""; //create a string to store the current word in
	for (int i = 0; i < line.size(); i++)
	{
		//only create words out of valid characters 
		if (isalpha(line[i]) || line[i] == '\'')
		{
			word += tolower(line[i]);
		}
		// if the line encounters any non valid character, then count check if the word before it is valid
		if (!isalpha(line[i]) && line[i] != '\'')
		{
			if (word.size() > 0)
			{
				if (m_root->search(m_root,word)) // look for word in dictionary using helper function
				{ }
				else
				{
					// if the word is not in the dictionary create a problem, store the start and end of the word into problem, push it onto problems
					Position problem;
					problem.start = i - (word.size());
					problem.end = i-1;
					problems.push_back(problem);
				}
				word = ""; //reset currword
			}
		}
		if (word.size() > 0 && i == line.size() - 1)
		{
			if (m_root->search(m_root, word)) // look for word in dictionary using helper function
			{
			}
			else
			{
				// if the word is not in the dictionary create a problem, store the start and end of the word into problem, push it onto problems
				Position problem;
				problem.start = i - (word.size()-1);
				problem.end = i;
				problems.push_back(problem);
			}
			word = ""; //reset currword
		}
	}

}
