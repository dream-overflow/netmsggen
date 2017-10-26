# Ident result message
msg IdentResult<2>
{
	@client in
	@server out

#	const int8 SUCCESS = 1
#	const int8 FAILED = 0

	int8 m_accept

#	set accept
#	{
#		m_accept = SUCCESS
#	}

#	set failed
#	{
#		m_accept = FAILED
#	}
}
