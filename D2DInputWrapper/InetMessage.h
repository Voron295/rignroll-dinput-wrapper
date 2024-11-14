#include "vector.h"

class InetMessage
{
	vector<char> message;
	int blocksCount;
	int field_14;
	int currentOffset;
	
	virtual int GetBlocksCount();
	virtual int GetSize();
};