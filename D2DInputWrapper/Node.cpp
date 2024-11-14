#include "Node.h"

Node::Node()
{
	m_pNext = 0;
	m_pPrev = 0;
}

Node::~Node()
{

}

Node *Node::Copy(Node *node)
{
	return ((Node *(__thiscall*)(Node *, Node *))0x5D2230)(this, node);
}

int Node::WriteChainToFile(FILE *f)
{
	return ((int(__thiscall*)(Node *, FILE *))0x5D2150)(this, f);
}

int Node::ReadChainFromFile(FILE *f)
{
	return ((int(__thiscall*)(Node *, FILE *))0x5D21A0)(this, f);
}

int Node::Write()
{
	return ((int(__thiscall*)(Node *))0x5D2270)(this);
}