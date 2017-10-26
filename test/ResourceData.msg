# Resource data message
import World

msg ResourceData<15> : World
{
	@client in
	@server out

	int16 m_resourceId
	bitset8 m_content

	if m_content[TYPE]
	{
		int16 m_resourceTypeId
	}

	if m_content[QUANTITY]
	{
		int8 m_direction
		int32 m_quantity
	}

	if m_content[MAX_QUANTITY]
	{
		int32 m_maxQuantity
	}
}
