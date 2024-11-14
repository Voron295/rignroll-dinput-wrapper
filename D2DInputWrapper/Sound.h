#ifndef SOUND_CLASS_H
#define SOUND_CLASS_H

class Sound
{
public:
	char *m_sName;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int m_dFileId;

	virtual ~Sound();
};

#endif