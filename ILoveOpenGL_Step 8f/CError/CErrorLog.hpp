#include "CErrorLog.h"



template <class T>
void CErrorLog::PrintToLog( T theThingToWrite, bool bWriteToCErr /*=false*/, bool bWriteToCout /*=false*/ )
{
	if ( this->m_bWriteToLogFile && this->m_pLogFile != 0 )
	{
		(*this->m_pLogFile) << theThingToWrite;
		this->m_PrintEndlineCharacter_toFile();
	}//if ( this->m_pLogFile != 0 )

	if ( this->m_bWriteToCerr || bWriteToCErr )
	{
		std::cerr << theThingToWrite;
		this->m_PrintEndlineCharacter_toCERR();
	}//if ( this->m_bWriteToCerr || bWriteToCErr )

	if ( this->m_bWriteToCout || bWriteToCout )
	{
		std::cout << theThingToWrite;
		this->m_PrintEndlineCharacter_toCOUT();
	}//if ( this->m_bWriteToCout || bWriteToCout )

	return;
}

template <>
// Specific type for strings...
void CErrorLog::PrintToLog( const std::string &theThingToWrite, bool bWriteToCErr /*=false*/, bool bWriteToCout /*=false*/ )
{
	if ( this->m_bIgnoreBlankStrings && theThingToWrite == "" )
	{
		return;
	}
	if ( this->m_bWriteToLogFile && this->m_pLogFile != 0 )
	{
		(*this->m_pLogFile) << theThingToWrite;
		this->m_PrintEndlineCharacter_toFile();
	}//if ( this->m_pLogFile != 0 )

	if ( this->m_bWriteToCerr || bWriteToCErr )
	{
		std::cerr << theThingToWrite;
		this->m_PrintEndlineCharacter_toCERR();
	}//if ( this->m_bWriteToCerr || bWriteToCErr )

	if ( this->m_bWriteToCout || bWriteToCout )
	{
		std::cout << theThingToWrite;
		this->m_PrintEndlineCharacter_toCOUT();
	}//if ( this->m_bWriteToCout || bWriteToCout )

	return;
}

template <>
// Specific type for char...
void CErrorLog::PrintToLog( const char* &theThingToWrite, bool bWriteToCErr /*=false*/, bool bWriteToCout /*=false*/ )
{
	if ( this->m_bIgnoreBlankStrings && (strcmp(theThingToWrite,"")==0) )
	{
		return;
	}
	if ( this->m_bWriteToLogFile && this->m_pLogFile != 0 )
	{
		(*this->m_pLogFile) << theThingToWrite;
		this->m_PrintEndlineCharacter_toFile();
	}//if ( this->m_pLogFile != 0 )

	if ( this->m_bWriteToCerr || bWriteToCErr )
	{
		std::cerr << theThingToWrite;
		this->m_PrintEndlineCharacter_toCERR();
	}//if ( this->m_bWriteToCerr || bWriteToCErr )

	if ( this->m_bWriteToCout || bWriteToCout )
	{
		std::cout << theThingToWrite;
		this->m_PrintEndlineCharacter_toCOUT();
	}//if ( this->m_bWriteToCout || bWriteToCout )

	return;
}


template <class T>
void CErrorLog::PrintToLog( T theThingToWrite, const int line, const char* file, bool bWriteToCErr /*=false*/, bool bWriteToCout /*=false*/ )
{
	if ( this->m_bWriteToLogFile && this->m_pLogFile != 0 )
	{
		if ( this->m_bAutoStripPathFromFileNames )
		{	(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
		else 
		{	(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
		this->m_PrintEndlineCharacter_toFile();
	}//if ( m_bWriteToLogFile && this->m_pLogFile != 0 )

	if ( this->m_bWriteToCerr || bWriteToCErr )
	{
		if ( this->m_bAutoStripPathFromFileNames )
		{	std::cerr << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
		else 
		{	std::cerr << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
		this->m_PrintEndlineCharacter_toCERR();
	}//if ( m_bWriteToLogFile && this->m_pLogFile != 0 )

	if ( this->m_bWriteToCout || bWriteToCout )
	{
		if ( this->m_bAutoStripPathFromFileNames )
		{	std::cout << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
		else 
		{	std::cout << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
		this->m_PrintEndlineCharacter_toCOUT();
	}//if ( m_bWriteToLogFile && this->m_pLogFile != 0 )

	return;
}

template <>	// Specific type for strings...
void CErrorLog::PrintToLog( const std::string &theThingToWrite, const int line, const char* file, bool bWriteToCErr /*=false*/, bool bWriteToCout /*=false*/ )
{
	if ( this->m_bIgnoreBlankStrings && theThingToWrite == "" )
	{
		return;
	}
	if ( this->m_bWriteToLogFile && this->m_pLogFile != 0 )
	{
		if ( this->m_bAutoStripPathFromFileNames )
		{	(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
		else 
		{	(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
		this->m_PrintEndlineCharacter_toFile();
	}//if ( m_bWriteToLogFile && this->m_pLogFile != 0 )

	if ( this->m_bWriteToCerr || bWriteToCErr )
	{
		if ( this->m_bAutoStripPathFromFileNames )
		{	std::cerr << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
		else 
		{	std::cerr << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
		this->m_PrintEndlineCharacter_toCERR();
	}//if ( m_bWriteToLogFile && this->m_pLogFile != 0 )

	if ( this->m_bWriteToCout || bWriteToCout )
	{
		if ( this->m_bAutoStripPathFromFileNames )
		{	std::cout << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
		else 
		{	std::cout << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
		this->m_PrintEndlineCharacter_toCOUT();
	}//if ( m_bWriteToLogFile && this->m_pLogFile != 0 )

	return;
}

template <>	// Specific type for char...
void CErrorLog::PrintToLog( const char* &theThingToWrite, const int line, const char* file, bool bWriteToCErr /*=false*/, bool bWriteToCout /*=false*/ )
{
	if ( this->m_bIgnoreBlankStrings && (strcmp(theThingToWrite,"")==0) )
	{
		return;
	}
	if ( this->m_bWriteToLogFile && this->m_pLogFile != 0 )
	{
		if ( this->m_bAutoStripPathFromFileNames )
		{	(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
		else 
		{	(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
		this->m_PrintEndlineCharacter_toFile();
	}//if ( m_bWriteToLogFile && this->m_pLogFile != 0 )

	if ( this->m_bWriteToCerr || bWriteToCErr )
	{
		if ( this->m_bAutoStripPathFromFileNames )
		{	std::cerr << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
		else 
		{	std::cerr << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
		this->m_PrintEndlineCharacter_toCERR();
	}//if ( m_bWriteToLogFile && this->m_pLogFile != 0 )

	if ( this->m_bWriteToCout || bWriteToCout )
	{
		if ( this->m_bAutoStripPathFromFileNames )
		{	std::cout << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
		else 
		{	std::cout << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
		this->m_PrintEndlineCharacter_toCOUT();
	}//if ( m_bWriteToLogFile && this->m_pLogFile != 0 )

	return;
}

template <class T>
void CErrorLog::PrintToLogNoPath( T theThingToWrite, const int line, const char* file, bool bWriteToCErr /*=false*/, bool bWriteToCout /*=false*/ )
{
	if ( this->m_bWriteToLogFile && this->m_pLogFile != 0 )
	{
		(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
		this->m_PrintEndlineCharacter_toFile();
	}

	if ( this->m_bWriteToCerr || bWriteToCErr )
	{
		std::cerr << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
		this->m_PrintEndlineCharacter_toCERR();
	}

	if ( this->m_bWriteToCout || bWriteToCout )
	{
		std::cout << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
		this->m_PrintEndlineCharacter_toCOUT();
	}	

	return;
}

template <>	// Specific type for strings...
void CErrorLog::PrintToLogNoPath( const std::string &theThingToWrite, const int line, const char* file, bool bWriteToCErr /*=false*/, bool bWriteToCout /*=false*/ )

{
if ( this->m_bIgnoreBlankStrings && theThingToWrite == "" )
{
	return;
}	
	if ( this->m_bWriteToLogFile && this->m_pLogFile != 0 )
	{
		(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
		this->m_PrintEndlineCharacter_toFile();
	}

	if ( this->m_bWriteToCerr || bWriteToCErr )
	{
		std::cerr << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
		this->m_PrintEndlineCharacter_toCERR();
	}

	if ( this->m_bWriteToCout || bWriteToCout )
	{
		std::cout << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
		this->m_PrintEndlineCharacter_toCOUT();
	}	

	return;
}

template <>	// Specific type for char...
void CErrorLog::PrintToLogNoPath( const char* &theThingToWrite, const int line, const char* file, bool bWriteToCErr /*=false*/, bool bWriteToCout /*=false*/ )

{
	if ( this->m_bIgnoreBlankStrings && (strcmp(theThingToWrite,"")==0) )
	{
		return;
	}	
	if ( this->m_bWriteToLogFile && this->m_pLogFile != 0 )
	{
		(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
		this->m_PrintEndlineCharacter_toFile();
	}

	if ( this->m_bWriteToCerr || bWriteToCErr )
	{
		std::cerr << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
		this->m_PrintEndlineCharacter_toCERR();
	}

	if ( this->m_bWriteToCout || bWriteToCout )
	{
		std::cout << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
		this->m_PrintEndlineCharacter_toCOUT();
	}	

	return;
}

