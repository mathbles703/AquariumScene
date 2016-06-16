#include "COpenGLError.h"

// Reutn false if everything is OK
//static 
bool COpenGLError::bWasThereAnOpenGLError(void)
{
	GLenum errorEnum = GL_NO_ERROR;
	std::string errorText;
	std::string errorDetails;
	return COpenGLError::bWasThereAnOpenGLError( errorEnum, errorText, errorDetails );
}


//static 
bool COpenGLError::bWasThereAnOpenGLError(GLenum &errorEnum)
{
	std::string errorText;
	std::string errorDetails;
	return COpenGLError::bWasThereAnOpenGLError( errorEnum, errorText, errorDetails );
}

//static 
bool COpenGLError::bWasThereAnOpenGLError(GLenum &errorEnum, std::string &errorString)
{
	std::string errorDetails;
	return COpenGLError::bWasThereAnOpenGLError( errorEnum, errorString, errorDetails );
}

//static 
bool COpenGLError::bWasThereAnOpenGLError(std::string &errorText, std::string &errorDetails)
{
	GLenum errorEnum = GL_NO_ERROR;
	return COpenGLError::bWasThereAnOpenGLError( errorEnum, errorText, errorDetails );
}

// These require an error logger class to be passed, too
//static 
bool COpenGLError::LogOpenGLErrorIfPresent(CErrorLog &errorLogger )
{
	std::string errorText;
	std::string errorDetails;
	bool bWasError = COpenGLError::bWasThereAnOpenGLError( errorText, errorDetails );
	if ( bWasError )
	{	// Sorry, but this is because the stream operator isn't properly overloaded
		errorLogger << errorText;
		errorLogger << " ";
		errorLogger << errorDetails;
	}
	return bWasError;
}

//static 
bool COpenGLError::LogOpenGLErrorIfPresent(std::string prependText, CErrorLog &errorLogger )
{
	std::string errorText;
	std::string errorDetails;
	bool bWasError = COpenGLError::bWasThereAnOpenGLError( errorText, errorDetails );
	if ( bWasError )
	{	// Sorry, but this is because the stream operator isn't properly overloaded
		std::stringstream ssError;
		ssError << prependText << ":" << errorText << "\n\tDetails:" << errorDetails;
		errorLogger.PrintToLog( ssError.str() );
	}
	return bWasError;
}

//static 
bool COpenGLError::LogOpenGLErorrIfPresent(const int line, const char* file, CErrorLog &errorLogger )
{
	std::string errorText;
	std::string errorDetails;
	bool bWasError = COpenGLError::bWasThereAnOpenGLError( errorText, errorDetails );
	if ( bWasError )
	{	// Sorry, but this is because the stream operator isn't properly overloaded
		std::stringstream ssError;
		ssError << errorText << "\n\tDetails:" << errorDetails;
		errorLogger.PrintToLog( ssError.str(), line, file );
	}
	return bWasError;
}

//static 
bool COpenGLError::LogOpenGLErorrIfPresent(std::string prependText, const int line, const char* file, CErrorLog &errorLogger )
{
	std::string errorText;
	std::string errorDetails;
	bool bWasError = COpenGLError::bWasThereAnOpenGLError( errorText, errorDetails );
	if ( bWasError )
	{	// Sorry, but this is because the stream operator isn't properly overloaded
		std::stringstream ssError;
		ssError << prependText << ":" << errorText << "\n\tDetails:" << errorDetails;
		errorLogger.PrintToLog( ssError.str(), line, file );
	}
	return bWasError;
}

//static 
bool COpenGLError::bWasThereAnOpenGLError(GLenum &errorEnum, std::string &errorText, std::string &errorDetails)
{
#ifdef _DEBUG		// Visual studio specific
	errorEnum = glGetError();

	// from: https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGetError.xml
	switch ( errorEnum )
	{
	case GL_NO_ERROR:
		errorText = "GL_NO_ERROR";
		errorDetails = "GL_NO_ERROR: No error has been recorded.";
		/******************/
		return false;
		/******************/
		break;
	case GL_INVALID_ENUM:
		errorText = "GL_INVALID_ENUM"; 
		errorDetails = "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
		break;
	case GL_INVALID_VALUE:
		errorText = "GL_INVALID_VALUE"; 
		errorDetails = "GL_INVALID_VALUE: A numeric argument is out of range.";
		break;
	case GL_INVALID_OPERATION:
		errorText = "GL_INVALID_OPERATION"; 
		errorDetails = "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		errorText = "GL_INVALID_FRAMEBUFFER_OPERATION"; 
		errorDetails = "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
		break;
	case GL_OUT_OF_MEMORY:
		errorText = "GL_OUT_OF_MEMORY"; 
		errorDetails = "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
		break;
	case GL_STACK_UNDERFLOW:
		errorText = "GL_STACK_UNDERFLOW"; 
		errorDetails = "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.";
		break;
	case GL_STACK_OVERFLOW:
		errorText = "GL_STACK_OVERFLOW"; 
		errorDetails = "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow.";
		break;
	default:
		errorText = "Unknown error";
		errorDetails = "Unknown error";
		break;
	};

	return true;	// There -WAS- an error
#else
	// DON'T call glGetError() if in release
	errorText = "glGetError() call bypassed in release build";
	errorDetails = "glGetError() call bypassed in release build";

	return false;
#endif
}

