#include "StudentUndo.h"
using namespace std;

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	UndoObject u(action, row, col, ch);
	if (!m_undoStack.empty()) //only do this if there was a previous item on stack to check for batching
	{	
		//For batching delete if both current and prev action are Delete and they happenned in the same row
		if (action == DELETE && m_undoStack.top().action == action && row == m_undoStack.top().row)
		{
			//only batch if the deletes happened either in the same spot or one before the other
			int distance = col - m_undoStack.top().col;
			if (distance == -1)
			{
				//if the delete happenned before
				m_undoStack.top().content = ch + m_undoStack.top().content; //append ch before content
				m_undoStack.top().row = u.row; //update row and col appropriately
				m_undoStack.top().col = u.col;
			}
			else if (distance == 0) //if happenned in the same position
			{
				m_undoStack.top().content = m_undoStack.top().content += ch; //append ch after content
				m_undoStack.top().row = u.row; //update row and col appropriately
				m_undoStack.top().col = u.col;
			}
			else //otherwise push delete onto stack as is
				m_undoStack.push(u);
		}
		//For batching Insert if both current and prev action are Insert and they happenned in the same row
		else if (action == INSERT && m_undoStack.top().action == action && ch != ' ' && row == m_undoStack.top().row)
		{
			//find distance of inserts, inserts should only be batched if the new letter was inserted after the previous letter
			int prev_col = (m_undoStack.top().col + m_undoStack.top().content.size())-1;
			int distance = col - prev_col; //find distance between current letter and previous letter
			if (col < prev_col)
				distance += 1;
			if (distance == 1) //batch if the distance is one
			{
				if (ch == '\t') //special case for tab
					m_undoStack.top().content += "    ";
				else
					m_undoStack.top().content.push_back(ch);
			}
			else //otherwise push insert onto stack as is
				m_undoStack.push(u);
		}
		else //otherwise push U onto stack as is
			m_undoStack.push(u);
	}
	else //otherwise push U onto stack as is
		m_undoStack.push(u);
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
    if(m_undoStack.empty())
		return Action::ERROR; 
	row = m_undoStack.top().row;
	col = m_undoStack.top().col; // set row and col to the top of the stacks row and col
	count = 1; //for most cases count =1
	text = ""; //for most cases text = ""
	if (m_undoStack.top().action == INSERT)
	{
		count = m_undoStack.top().content.size(); //set count to size of string 
		m_undoStack.pop(); //pop top of stack
		return DELETE;
	}
	if (m_undoStack.top().action == DELETE)
	{
		text = m_undoStack.top().content; //set text stack top's string
		m_undoStack.pop(); //pop top of stack
		return INSERT;
	}
	if (m_undoStack.top().action == JOIN)
	{
		m_undoStack.pop(); //pop top of stack
		return SPLIT;
	}
	if (m_undoStack.top().action == SPLIT)
	{
		m_undoStack.pop(); //pop top of stack
		return JOIN;
	}
	return Action::ERROR;
}

void StudentUndo::clear() 
{
	if (m_undoStack.empty()) //if the stack is empty do nothing
		return;
	while (!m_undoStack.empty()) //otherwise pop every undo object off the stack
	{
		m_undoStack.pop();
	}
}
