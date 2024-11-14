#ifndef TASKMANAGER_CLASS_H
#define TASKMANAGER_CLASS_H
#include "Task.h"

class TaskManager
{
public:
	int tasksProcessed;
	Task *m_pVehicleTasks;
	TaskNode *field_8;
	TaskNode *tasks;
	int field_10;
	int field_14;
	int m_pTimePassed;
};

#endif