//////////////////////////////////////////////////////////////////////////
/**
 * support lib basic debug and error functions...
 *	\file		rddebug.h
 *	\author		bombur
 *	\version	0.2
 *	\date		20.09.2000
 */
//////////////////////////////////////////////////////////////////////////

#ifndef RD_DEBUG_H
#define RD_DEBUG_H

#define RD_MEMDEBUG_OFF

/////////////////
/// Here can be defined engine debug mode - error logging system:
#ifndef RD_DEBUG_OFF
#  define RD_DEBUG
#endif // of RD_DEBUG_OFF

/// Switch on/off memory debugging
#ifndef RD_MEMDEBUG_OFF
#  define RD_MEMDEBUG
#endif //RD_MEMDEBUG_OFF
/////////////////

#include <support/rdmemory.h>

/// Returns available physical and virtual memory size
void RDGetFreeMemory(DWORD *phys, DWORD *virt);

/// used by RDASSERT/RDVERIFY
void RDFailAssert(char *expr,char *srcFile,int srcLine);

// Now the 'brilliant' code follows for trace purposes...
// Yes, waiting for C99-standard macros support...

/// Message types
enum 
{
	RDMES_ERROR = 0,		/// Fatal error - function aborts
	RDMES_WARNING = 1,		/// Warning - something is wrong
	RDMES_DEBUG = 2			/// Debug message (logging)
};
const int RDDEBUG_NUMMESTYPES = 3;

/// use RDMES macros for function call error/warning/debug output.
/// if '%err' is in message - attempts message output using platform-specific (Win32, DX, OGL etc.) methods.
#define RDERRMES RDTrace(__FILE__, __LINE__, RDMES_ERROR)
#define RDWARNMES RDTrace(__FILE__, __LINE__, RDMES_WARNING)
#define RDDEBUGMES RDTrace(__FILE__, __LINE__, RDMES_DEBUG)

/// obsolete
#define RDMES RDERRMES

/// use RDCHECK for function call error check (attempts message output using platform-specific (Win32, DX, OGL etc.) methods.)
#define RDCHECK(function) debug->Check(__FILE__, __LINE__, #function, function)

/// use RDCHECKMSG for function call error check, and message output when error.
#define RDCHECKMSG(function, mes) debug->Check(__FILE__, __LINE__, #function, function, mes)

/// simple assert macros
/// use RDASSERT for verifying assertions during debug
#ifdef RD_DEBUG
#define RDASSERT(expr) {if(!(expr)) RDFailAssert( #expr, __FILE__, __LINE__ );}
#else
#define RDASSERT(expr)
#endif
/// use RDVERIFY for verifying assertions
#ifdef RD_DEBUG
#define RDVERIFY(expr) {if(!(expr)) RDFailAssert( #expr, __FILE__, __LINE__ );}
#else
#define RDVERIFY(expr) (expr)
#endif


/// Debug message output destination types
enum
{
	RDMES_OUTPUT_NONE     = 0,	/// Silent mode
	RDMES_OUTPUT_LOGFILE  = 1,	/// Write log file (=default for all message types)
	RDMES_OUTPUT_DEBUGGER = 2,	/// Output message to debugger window
	RDMES_OUTPUT_CONSOLE  = 3	/// Open console window for message output
};
const int RDDEBUG_NUMDESTS = 4;


/// abstract debug manager class
class RDDebug
{
public:
	/// dtor
	virtual ~RDDebug() {}
	/// Set output destination for specified message type (see enums RDMES_* above)
	virtual BOOL    SetDebugOutput(int mestype, int dest) = 0;
	/// Check given function for errors (HRESULT type)
	virtual HRESULT Check (const char *filename, int line, const char *funcname, HRESULT Result, const char *message = NULL) = 0;
	/// Check given function for errors (boolean type)
	virtual BOOL	Check (const char *filename, int line, const char *funcname, BOOL Result, const char *message = NULL) = 0;
	/// Check given function for errors (pointer type)
	virtual void	*Check (const char *filename, int line, const char *funcname, void *Result, const char *message = NULL) = 0;

	/// Output error/warning message.
	/// Interprets %err tag as system error message and '!' at the beginning as exception throw
	virtual void	OutputMessage (const char *filename, int line, const char *funcname, int type, const char *message = NULL, va_list vl = NULL) = 0;
	
	/// use debug message and logging subsystem (user-defined messages)
	virtual void	DebugMsg(int dest, char *message, ...) = 0;
	/// use debug message and logging subsystem for indexed errors
	virtual void	DebugMsg(int n = 0) = 0;

	/// show error in message box
	virtual void	ShowError() = 0;
	/// get last error code (code-dependent) - for code and error text connection
	virtual DWORD	GetErrorCode() = 0;
	/// get last error string
	virtual char	*GetError() = 0;
};


RDDebug *CreateDebug();
BOOL DeleteDebug(RDDebug *);


/// global debug variable
extern RDDebug *debug;


/// Debug Tracer class. Message output
class RDTrace
{
public:
	/// ctor used by macros
	RDTrace (const char *file, int line, int typ)
	{
		filename = file;
		lineno = line;
		type = typ;
	}

	/// for variable arguments macro calls
	void operator() (const char *fmt, ...) 
	{
        va_list l;
		va_start(l, fmt);
		if (debug != NULL)
			debug->OutputMessage(filename, lineno, NULL, type, fmt, l);
    }

private:
	/// message type
	int type;
	/// line number where message was raised
	int lineno;
	/// source file name
	const char *filename;
};

/////////////////////////////////////////////////////////////////////////
// Standard Debug Messages

/// 'standard' memory error string
const char MEMERR[] = "Memory allocation error.";

#endif // of RD_DEBUG_H
