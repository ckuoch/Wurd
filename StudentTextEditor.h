#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include "Undo.h"
#include <string>
#include <list>
#include<iterator>
class Undo;

class StudentTextEditor : public TextEditor {
public:

	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:
	std::list<std::string> m_lines;
	std::list<std::string>::iterator m_currentLine;
	int m_row;
	int m_col;
	void getRow()
	{
		// only do something if m_lines is not empty
		if (m_lines.size() > 0)
		{
			// convert iterator to int value
			int curr = std::distance(m_lines.begin(), m_currentLine);
			//calculate distance of current iterator line from m_row
			int distance = curr - m_row;
			if (distance!=0)
			{
				// if distance is greater
				if (distance > 0)
				{
					for (int i = 0; i < distance; i++)
					{
						//decrement current line until it reaches m_row
						m_currentLine--;
					}
				}
				else // distance is less
				{
					for (int i = 0; i < (distance * -1); i++)
					{
						//increment current line until it reaches m_row
						m_currentLine++;
					}
				}
			}
		}
	}
};

#endif // STUDENTTEXTEDITOR_H_
