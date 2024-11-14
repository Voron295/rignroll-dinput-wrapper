struct PositionId
{
	int roadid;
	void *nodeid;
	double field_8;
	int roadSegmentVectorId;
	int roadSegmentId;
	int field_18;
	int field_1C;
};

static_assert (sizeof(PositionId) == 0x20, "Size is not correct");