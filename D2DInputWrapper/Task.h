#ifndef TASK_CLASS_H
#define TASK_CLASS_H
#include "Node.h"

class Task
{
public:
	void *_vmt;
	Task *m_pPrev;
	Task *m_pNext;
	class TaskManager *m_pTaskManager;
	int field_10;
	int m_dwTaskId;
	int field_18;
	char *m_sName;
};


struct TaskNode : public Node
{
	Task *task;
	int tasksCount;
	int field_14;
};

#endif