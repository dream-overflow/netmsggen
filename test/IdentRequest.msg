# Ident request
msg IdentRequest<2>
{
	@client out
	@server in

	array8 m_ident

#	set ident(string ident, array8 challenge)
}
