#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <string>
#include <stack>

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct UndoObject // make a struct to store all of undos info
	{
		UndoObject(Action action, int row, int col, char ch) : action(action), row(row), col(col) 
		{
			if (ch == '\t')
				content += "    ";
			else
				content += ch;
		};
		Action action;
		std::string content;
		int row;
		int col;
	};
	std::stack<UndoObject> m_undoStack;
};

#endif // STUDENTUNDO_H_
