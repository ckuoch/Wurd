#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo), m_row(0), m_col(0) {
	// initialize m_lines and m_currentline so no text editor
	m_lines.push_back("");
	m_currentLine = m_lines.begin();
}

StudentTextEditor::~StudentTextEditor()
{}

bool StudentTextEditor::load(std::string file) {
	ifstream infile(file);
	if (!infile) // if file doesnt load
		return false; 
	else
	{
		reset(); // reset text editor to default pos
		string s;
		while (getline(infile, s)) // read in every line document
		{
			if (s!= "" && s.back() == '\r') // search for '\r' and elimnate it
			{
				s.pop_back();
			}
			m_lines.push_back(s); //push back current line
		}
		m_currentLine = m_lines.begin();
		return true;
	}
}

bool StudentTextEditor::save(std::string file) {
	ofstream outfile(file);
	if (!outfile)
		return false;
	else
	{
		for (list<string>::iterator p = m_lines.begin(); p != m_lines.end(); p++) //output each line + a new line
		{
			outfile << (*p) + '\n';
		}
		return true;
	}
}

void StudentTextEditor::reset() {
	//clear lines if not empty
	if(!m_lines.empty())
		m_lines.clear();
	// reset to initial position
	m_row = 0; 
	m_col = 0;
	//clear undo
	getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
	//find out where the last line is
	list<string>::iterator lastLine = m_lines.end();
	lastLine--;
	switch (dir)
	{
	case Dir::UP:
	{
		// move up one line if not at the top line
		if (m_currentLine != m_lines.begin())
		{
			// move row and Currline iterator up
			m_row--;
			m_currentLine--;
			// if the col is further than the last character of the new row, move the col
			if (m_col > m_currentLine->size())
			{
				m_col = m_currentLine->size();
			}
			break;
		}
		break;
	}
	case Dir::DOWN:
	{ // move down one line if not at the top line

		if (m_currentLine != lastLine)
		{
			// move row and Currline iterator down
			m_row++;
			m_currentLine++;
			// if the col is further than the last character of the new row, move the col
			if (m_col > m_currentLine->size())
			{
				m_col = m_currentLine->size();
			}
			break;
		}
		break;
	}
	case Dir::LEFT:
	{
		// if youre at the very start of a line and want to move left, you need to move up a row and change col to the end of that row
		if (m_col == 0)
		{
			//don't do this if you are at the first line
			if (m_currentLine == m_lines.begin())
			{
				break;
			}
			//move up
			m_row--;
			m_currentLine--;
			// change col to end of line
			m_col = m_currentLine->size();
			break;
		}
		// otherwise move col over one
		m_col--;
		break;
	}
	case Dir::RIGHT:
	{
		// if youre at the very end of a line and want to move right, you need to move down a row and change col to the start of that row
		if (m_col == m_currentLine->size())
		{
			// dont do this if you are on last line
			if (m_currentLine == lastLine)
			{
				break;
			}
			// move down
			m_row++;
			m_currentLine++;
			//set col to first char
			m_col = 0;
			break;
		}
		// otherwise move col over one
		m_col++;
		break;
	}
	case Dir::HOME:
	{
		//set col to beginning
		m_col = 0;
		break;
	}
	case Dir::END:
	{
		// set col to end
		m_col = m_currentLine->size();
		break;
	}
	default: break;
	}
}

void StudentTextEditor::del() {
	// Give the last line a variable
	list<string>::iterator lastLine = m_lines.end();
	lastLine--;
	string s = *m_currentLine; // store the string of the currentline
	// if the you delete the last character of a line, merge it with the line below
	if (m_col == m_currentLine->size())
	{
		//dont do this if youre on the last line
		if (m_currentLine == lastLine)
		{
			return;
		}
		//otherwise delete the current line and append the string to the next line
		m_currentLine = m_lines.erase(m_currentLine);
		*m_currentLine = s + *m_currentLine;
		// submit info to the undo object
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
	}
	else
	{
		//otherwise delete the char at the cursor position
		char deleted = s[m_col];
		*m_currentLine = m_currentLine->substr(0, m_col) + m_currentLine->substr(m_col+1, m_currentLine->size());
		// submit info to the undo object
		getUndo()->submit(Undo::Action::DELETE , m_row, m_col, deleted);
	}
	
}

void StudentTextEditor::backspace() {
	if (m_col > 0)
	{
		//if m_col is not zero then move col left by one and call del to erase the character that character
		m_col--;
		del();
	}
	if (m_col == 0 && m_currentLine != m_lines.begin())
	{
		//if on the edge of a line and that line is not the beginning line
		string s = *m_currentLine; // store string of current line
		m_currentLine = m_lines.erase(m_currentLine); //erase the current line
		m_currentLine--; //decrement currentline since you are joining with the line above
		m_col = m_currentLine->size(); //set col to the new row end
		m_row--; //derement row to match
		*m_currentLine += s; // append s to current line
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col); //submit info
	}
}

void StudentTextEditor::insert(char ch) {
	string s;
	//if the char is tab then convert to 4 spaces
	if (ch == '\t')
	{
		s += "    ";
		m_currentLine->insert(m_col, s); //string insert the 4 spaces
		m_col++;
		getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);	//submit info
		m_col += 3;// move right 4 times
	}
	else {
		m_currentLine->insert(m_col, s + ch); // insert ch
		m_col++; //move right once
		getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);	//submit info
	}
}

void StudentTextEditor::enter() {
	// submit info before moving
	getUndo()->submit(Undo::Action::SPLIT, m_row, m_col);
	string s = m_currentLine->substr(m_col, m_currentLine->size()-1); // store substring 
	if (m_col == 0)
	{
		s = *m_currentLine;
	}
	list<string>::iterator newLine = m_currentLine;
	newLine++; // create an iterator for the next line
	m_lines.insert(newLine, s); // insert new line with string s
	*m_currentLine = m_currentLine->substr(0, m_col); // update curr string
	move(Dir::RIGHT); //move the cursor to the right which will move it to the start of the next line
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_row;
	col = m_col;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if ((startRow < 0 || numRows < 0) || startRow > m_lines.size())
		return -1;
	lines.clear(); // clear lines
	int currRow = 0;
	int distance = 0;
	getPos(currRow, distance); //get current row and use distance as a place holder
	distance = currRow - startRow; //find out distance betwen currRow and startRow
	list<string>::const_iterator p = m_currentLine;
	if (distance > 0)
	{
		for (int i = 0; i < distance; i++)
			p--; //move to startrow
	}
	if (distance < 0)
	{
		for (int i = 0; i < distance; i++)
			p++; //move to start row
	}
	if (numRows > m_lines.size() - startRow) // if there are more rows than available
	{
		for (; p != m_lines.end(); p++)
		{
			lines.push_back(*p); // push every line onto line
		}
	}
	else //other wise only push the specified number of lines
	{
		for (int i = 0; i < numRows; i++)
		{
			lines.push_back(*p);
			p++;
		}
	}
	return lines.size(); 
}

void StudentTextEditor::undo() {
	int UndoRow;
	int UndoCol;
	int UndoCount;
	string text;
	//get action for switch statement and also get all info from stack using above variables
	Undo::Action a = getUndo()->get(UndoRow, UndoCol, UndoCount, text);
	switch (a)
	{
	case Undo::Action::INSERT:
	{
		m_row = UndoRow;
		m_col = UndoCol;
		getRow(); // get correct row
		m_currentLine->insert(m_col, text); //insert text at proper position
		break;
	}
	case Undo::Action::DELETE:
	{
		m_row = UndoRow;
		m_col = UndoCol-1;
		getRow(); // get correct row
		m_currentLine->erase(m_col, UndoCount); //erase from current string the length of the undo stack string
		break;
	}
	case Undo::Action::SPLIT:
	{
		if (m_row > UndoRow) // if the current row is below the undo row
		{
			m_row = UndoRow;
			m_col = UndoCol;
			getRow(); // get correct row
			string s(*m_currentLine);
			m_currentLine = m_lines.erase(m_currentLine); //erase current line
			*m_currentLine = s + *m_currentLine; //merge with below line
		}
		else //current row is above
		{
			m_col = UndoCol;
			//create iterator to a new line and insert the string into the next line
			string s = m_currentLine->substr(m_col, m_currentLine->size() - 1);
			list<string>::iterator newLine = m_currentLine;
			newLine++;
			m_lines.insert(newLine, s);
			*m_currentLine = m_currentLine->substr(0, m_col);
			move(Dir::RIGHT); //move right
		}
		break;
	}
	case Undo::Action::JOIN:
	{
		string s(*m_currentLine);
		m_currentLine = m_lines.erase(m_currentLine); //erase the currentline
		m_row = UndoRow;
		m_col = UndoCol;
		getRow(); // get the correct row
		m_currentLine->append(s); //append the string to the new line
		break;
	}
	//otherwise break
	case Undo::Action::ERROR:
	default:
		break;
	}
}
