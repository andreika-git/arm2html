//////////////////////////////////////////////////////////////////////////
/**
 * support lib memory debugging routines
 *	\file		rdmemory.h
 *	\author		bombur
 *	\version	0.1
 *	\date		14.07.2002 (23.08.2001)
 */
//////////////////////////////////////////////////////////////////////////

#ifndef RD_MEMORY_H
#define RD_MEMORY_H

// The memory remains... (c) M.
#ifdef RD_MEMDEBUG

/// allocation types
enum
{
	RD_ALLOC_UNKNOWN = 0,
	RD_ALLOC_NEW,
	RD_ALLOC_NEW_ARRAY,
	RD_ALLOC_MALLOC,
	RD_ALLOC_CALLOC,
	RD_ALLOC_REALLOC,
	RD_ALLOC_STRDUP,
	RD_ALLOC_DELETE,
	RD_ALLOC_DELETE_ARRAY,
	RD_ALLOC_FREE
};

/// internal stats
struct RDMemoryStats
{
	DWORD	totalReportedMemory;
	DWORD	totalActualMemory;
	DWORD	peakReportedMemory;
	DWORD	peakActualMemory;
	DWORD	accumulatedReportedMemory;
	DWORD	accumulatedActualMemory;
	DWORD	accumulatedAllocUnitCount;
	DWORD	totalAllocUnitCount;
	DWORD	peakAllocUnitCount;
};

/// internal memory manager's struct
struct RDAllocUnit
{
	DWORD		actualSize;
	DWORD		reqSize;
	void		*actualAddress;
	void		*reportedAddress;
	char		sourceFile[40];
	DWORD		sourceLine;
	DWORD		allocationType;
	DWORD		allocationNumber;
	RDAllocUnit	*next;
	RDAllocUnit	*prev;
};

const DWORD RD_MM_HASHSIZE				= 4096;
const DWORD RD_MM_HASHSIZEM				= RD_MM_HASHSIZE - 1;
static const char* RD_MM_LOGFILENAME	= "rd_mem.log";
static char *allocTypes[] = {"unknown", "new", "new[]", "malloc",
							"calloc", "realloc", "strdup", "delete",
							"delete[]",	"free"};

//////////////////////////////////////////////////////////////////////////
/// Memory allocator/tracer
class RDMemoryManager
{
public:
	/// Functions

	/// dtor. we use it to let us know when we're in static deinit.
	/// and log all leaks info
	~RDMemoryManager() {LeakReport();}

	/// singleton-related function
	static RDMemoryManager& GetHandle() {return mm;}

	/// basic mem functions
	void *Alloc(const char *file, const DWORD line,	const DWORD reqSize,
		const DWORD allocType);
	void *Realloc(const char *file, const DWORD line, const DWORD reqSize,
		void* reqAddress, const DWORD reallocType);
	void Dealloc(const char *file, const DWORD line, 
		const DWORD deallocType, void* reqAddress);
	char *Strdup(const char *file, const DWORD line, const char *source);

	/// final leak report
	void LeakReport();
	/// dump all current allocations
	void DumpAllocs(FILE *fp);
	/// print memory usage statistics
	void LogStats();
	/// set file and line number
	void SetOwner(const char *file, const DWORD line);
	
	// friends
	friend void *operator new(size_t reqSize);
	friend void *operator new[](size_t reqSize);
	friend void operator delete(void *reqAddress);
	friend void operator delete[](void *reqAddress);

private:
	/// Variables
	char *srcFile;							/// source file of call
	DWORD srcLine;							/// line number in scrFile
	
	RDAllocUnit *units;						/// allocation units array
	RDAllocUnit **unitsBuffer;				/// list linked to hash
	RDAllocUnit	*hashTable[RD_MM_HASHSIZE];	/// hash for fast unit search
	RDMemoryStats memStats;					/// memory stats

	DWORD currentAllocated;					/// currently allocated bytes
	DWORD unitsBufferSize;					/// number of allocated units
	DWORD borderSize;						/// size of safe border

	DWORD prefixPattern;					/// beginning pattern
	DWORD postfixPattern;					/// ending pattern
	DWORD unusedPattern;					/// clean memory pattern
	DWORD releasedPattern;					/// deleted memory pattern
	
	static RDMemoryManager mm;				// singleton

	/// Functions

	/// ctor. cleans logfile
	RDMemoryManager();

	/// create logfile
	void BeginLog();
	/// preserve globals
	void ClearGlobals();
	/// handle out-of-memory case
	void OutOfMemory();

	/// misc. functions

	/// fill allocated memory with specified pattern
	void FillWithPattern(RDAllocUnit *u, DWORD pattern, 
		DWORD originalSize = 0);
	/// internal: search unit in hash
	RDAllocUnit *FindAllocUnit(void *reqAddress);
	/// returnsname of src file
	char *GetSourceName(char *sourceFile);
	/// scan memory chunk for unused bytes (based on patterns)
	DWORD CalculateUnused(RDAllocUnit *u);
	/// validate integrity of memory allocation unit
	BOOL ValidateUnit(RDAllocUnit *u);
	/// helper function that logs into RD_MM_LOGFILENAME
	void Log(char *message, ...);
	/// dump unit
	void DumpUnit(RDAllocUnit *u);
};

// Operators' redefinition
void	*operator new(size_t reqSize);
void	*operator new[](size_t reqSize);
void	operator delete(void *reqAddress);
void	operator delete[](void *reqAddress);

#else // RD_MEMDEBUG
 
// just use RTL-fuctions
#define RDmalloc(s)		malloc(s)
#define RDalloca(s)		_alloca(s)
#define RDcalloc(s)		calloc(s, 1)
#define RDrealloc(d, s) realloc(d, s)
#define RDfree(d)		free(d)
#define RDstrdup(p)		strdup(p)
#define RDLogMemStats()

#endif // RD_MEMDEBUG

// Safe deallocation macros
#define RDSafeFree(ptr)			{if (ptr) RDfree(ptr); (ptr) = NULL;}
#define RDSafeDelete(ptr)		{if (ptr) delete ptr; (ptr) = NULL;}
#define RDSafeDeleteArray(ptr)	{if (ptr) delete [] ptr; (ptr) = NULL;}

#endif // of RD_MEMORY_H


// BELOW: can be included several times for avoiding colliding defs. in some header-files

#ifdef RD_MEMDEBUG
// Macros (can be called several times
#ifndef new

#define	new					(RDMemoryManager::GetHandle().SetOwner  (__FILE__, __LINE__), false) ? NULL : new
#define	delete				(RDMemoryManager::GetHandle().SetOwner  (__FILE__, __LINE__), false) ? NULL : delete
#define	RDmalloc(sz)		RDMemoryManager::GetHandle().Alloc(__FILE__, __LINE__, sz, RD_ALLOC_MALLOC)
#define RDalloca(s)			_alloca(s)
#define	RDcalloc(sz)		RDMemoryManager::GetHandle().Alloc(__FILE__, __LINE__, sz, RD_ALLOC_CALLOC)
#define	RDrealloc(ptr,sz)	RDMemoryManager::GetHandle().Realloc(__FILE__, __LINE__, sz, ptr, RD_ALLOC_REALLOC)
#define	RDfree(ptr)			RDMemoryManager::GetHandle().Dealloc(__FILE__, __LINE__, RD_ALLOC_FREE, ptr)
#define	RDstrdup(str)		RDMemoryManager::GetHandle().Strdup(__FILE__, __LINE__, str)
#define RDLogMemStats()		RDMemoryManager::GetHandle().LogStats();
#endif

#endif
