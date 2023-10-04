////////////////////////////////////////
// ARM2HTML Disassembler.
// (C) bombur, 2004.


#include <stdlib.h>
#include <stdio.h>
#include <isa.h>
#include <elf.h>

#include <support/hashlist.h>

//#define NOADDR

// The condition codes
const static char *cond[] = {	"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", 
								"hi", "ls", "ge", "lt", "gt", "le", "",   "nv"};

const static char *regs[] = {	"r0", "r1", "r2",  "r3", "r4", "r5", "r6", "r7", 
								"r8", "r9", "r10", "fp", "ip", "sp", "lr", "pc"};

const static char *cregs[] = {	"cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7", 
								"cr8", "cr9", "cr10", "cr11", "cr12", "cr13", "cr14", "cr15"};

const static char *dpiops[] = {	"and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc", 
								"tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"};

const static char *shift[] = {	"lsl", "lsr", "asr", "asl", "ror", "rrx"};

const static char *mult[] = {	"mul", "mla", "???", "???", "umull", "umla", "smull", "smlal"};

#include "syscalls.h"

// Multiplication opcodes
#define MUL   0
#define MLA   1
#define UMULL 4
#define UMLAL 5
#define SMULL 6
#define SMLAL 7

typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;


#define FLAT_FLAG_RAM    0x0001 // load program entirely into RAM
#define FLAT_FLAG_GOTPIC 0x0002 // program is PIC with GOT
#define FLAT_FLAG_GZIP   0x0004 // all but the header is compressed


DWORD filelen;

bool FLT = false, ELF = false;

RDList<int> starti;
int data_start = 0;

struct header
{
	char magic[4];
	unsigned long rev;          // version 
	unsigned long entry;        // Offset of first executable instruction
	                            // with text segment from beginning of file
	unsigned long data_start;   // Offset of data segment from beginning of file 
	unsigned long data_end;     // Offset of end of data segment from beginning of file 
	unsigned long bss_end;      // Offset of end of bss segment from beginning of file 

	// (It is assumed that data_end through bss_end forms the bss segment.)

	unsigned long stack_size;   // Size of stack, in bytes 
	unsigned long reloc_start;  // Offset of relocation records from beginning of file 
	unsigned long reloc_count;  // Number of relocation records
	unsigned long flags;       
	unsigned long filler[6];    // Reserved, set to zero
};

WORD ReadWordNet(BYTE* &fp)
{
	WORD w = *(fp++) << 8;
	w |= *(fp++);
	return w;
}

DWORD ReadDwordNet(BYTE* &fp)
{
	DWORD d = ReadWordNet(fp) << 16;
	d |= ReadWordNet(fp);
	return d;
}

RDDebug *debug = NULL;

class reloc_pair
{
public:
	reloc_pair() {}
	reloc_pair(int a, int o) { addr = a; offs = o; name = NULL; type = 0; }
	reloc_pair(int a, int o, char *n) { addr = a; offs = o; name = n; type = 0; }
	reloc_pair(int a, int o, char *n, int t) { addr = a; offs = o; name = n; type = t; }

	void CalcAddr(int startoff);

	int addr, offs;
	char *name;		// for ELF
	int type;
	int symb;
};

class RDRelocHashList : public RDHashList<reloc_pair>
{
public:
	/// ctor
	RDRelocHashList()
	{
	}
	
	/// dtor
	~RDRelocHashList()
	{
	}

    /// Hash function - declare it in derived class.
	/// The return value should be in the range of [0..num-1]
	DWORD HashFunc(reloc_pair const & item)	// \todo	Get better function
	{
		return (DWORD)item.offs % num;
	}

    /// Comparision function - declare it in derived class
    BOOL Compare(reloc_pair const & item1, reloc_pair const & item2)
    {
    	return item1.offs == item2.offs;
    }
};

class range
{
public:
	range(int ii1, int ii2) { i1 = ii1; i2 = ii2; }

	bool operator == (const range & r)
	{
		return i1 == r.i1 && i2 == r.i2;
	}

	int i1, i2;
};

RDRelocHashList reloc_hash;
RDRelocHashList symb_hash;	// for ELF
RDIntHashList link_hash;
RDList<range> exclude;

////////////////////////////////////////////////////////////

header flth;
Elf32_Ehdr elfh;
BYTE *buf;
BYTE *curbuf;

DWORD armregs[16];
bool  setregs[16];

char strtmp[1024];

#define MAXSHIFT 36
#define MAXCONDSHIFT 20
#define MAXREPEAT 10

class record
{
public:
	char *str;
	int branch;
	int sys;
	char flag;	// code=0, data=1, visited code=2
	int cnt;		// for avoiding repeating pieces
};

record *pr = NULL;
int numpr = 0;
bool inpr = true;

bool html = false;

void pr_set(int p)
{
	numpr = p;
}

int prf(char *fmt, ...)
{
	char tmp[4024];
	va_list l;
	va_start(l, fmt);
	int ret = vsprintf(tmp, fmt, l);
	if (inpr)
	{
		if (pr[numpr].str == NULL)
		{
			pr[numpr].str = RDstrdup(tmp);
			pr[numpr].branch = 0;
		}
		else
		{
			pr[numpr].str = (char *)RDrealloc(pr[numpr].str, strlen(pr[numpr].str) + strlen(tmp) + 1);
			strcat(pr[numpr].str, tmp);
		}
	} else
		printf(tmp);
	return ret;
}

int prf2(char *fmt, ...)
{
	if (!html)
		return 0;
	char tmp[4024];
	va_list l;
	va_start(l, fmt);
	int ret = vsprintf(tmp, fmt, l);
	if (inpr)
	{
		if (pr[numpr].str == NULL)
		{
			pr[numpr].str = RDstrdup(tmp);
			pr[numpr].branch = 0;
		}
		else
		{
			pr[numpr].str = (char *)RDrealloc(pr[numpr].str, strlen(pr[numpr].str) + strlen(tmp) + 1);
			strcat(pr[numpr].str, tmp);
		}
	} else
		printf(tmp);
	return ret;
}

void pr_data(BYTE *cc)
{
	int j;
	prf2("<span class=data>");
	for (j = 0; j < 16; j++)
		prf("%02X ", cc[j]);
	prf("| ");
	for (j = 0; j < 16; j++)
		prf("%c", strchr("\x1f\t\r\n", cc[j]) == NULL ? cc[j] : '.');
	prf2("</span>");
}


/// Decodes a branch instruction.
void decode_branch(uint32_t i, uint32_t offs)
{
	INST inst;
	inst.raw = i;

	prf2("<span class=inst>");
	prf("b");  

	if (inst.branch.link != 0)
	{
		prf("l");
		setregs[0] = false;		// linked jump can potentially destroy r0
	}
  
	prf("%s", cond[inst.branch.cond]);
	prf2("</span>");
	prf("\t");

	int coffs = inst.branch.offset;
	if ((coffs & 0xf00000) != 0)
		coffs |= 0xff000000;
	int n = (coffs+1)*4+offs+4;
	link_hash.Add(n);
#ifdef NOADDR
	prf("xxxxxx");
#else
  
	reloc_pair *p = reloc_hash.Get(reloc_pair(0, offs));

	if (p != NULL)
	{
		n = p->addr;
	}

	char *nn = NULL;
	if (p != NULL && p->name != NULL)
	{
		nn = p->name;
	}
	if (nn == NULL || nn[0] == '\0')
	{
		reloc_pair *p2 = symb_hash.Get(reloc_pair(0, n));
		if (p2 != NULL)
			nn = p2->name;
	}
  
	bool wasa = false;
	if (nn == NULL || nn[0] == '\0')
	{
		prf2("<a class=link href=\"#%06x\">", n);
		wasa = true;
	}

	prf("%06x", n);

	if (nn != NULL && nn[0] != '\0')
	{
		prf("   ");
		if (nn[0] != '.')
		{
			prf2("<a class=link href=\"#%06x\">", n);
			wasa = true;
		}
		/*  prf("   <sym=%d (%s)> ", p->symb, p->type == 1 ? "PC24" : "ABS32");
		else */
		prf(html ? "&lt;" : "<");
		prf("%s", nn);
		prf(html ? "&gt;" : ">");
	}

	if (wasa)
		prf2("</a>");
	if (nn == NULL || nn[0] == '\0')
		prf("(%x)", inst.branch.offset);
	
	if (inst.branch.link == 0)
	{
		prf(" ");
		prf2("<span class=comm>");
		prf("; %s", n == (int)offs ? "return" : "jump");
		prf2("</span>");
	}
#endif
	if (inst.branch.link == 0)
	{
		if (inpr)
		{
			prf("\n");
			if (n == (int)offs)
			{
				if (html)
					prf2("<hr>");
				else
					prf("\n");
			}
			if (inst.branch.cond == 0xe)
				pr[numpr].branch = n;	// don't do jump, just return
			else
			{
				pr[numpr].branch = n;
				pr[numpr].sys = -2;	// process conditionals as pseudo-calls
			}
		}
	}
	if (inst.branch.link != 0)
	{
		if (inpr)
		{
			pr[numpr].branch = n;
			pr[numpr].sys = -1;		// call, not jump
		}
	}
}


/// Decode the software interrupt.
void decode_swi(uint32_t i)
{
	INST inst;
	inst.raw = i;
  
	prf2("<span class=inst>");
	prf("swi%s", cond[inst.swi.cond]);
	prf2("</span>");
	prf("\t%x", inst.swi.val);
	for (int j = 0; syscall_desc[j].str != NULL; j++)
	{
		if (syscall_desc[j].num == (int)inst.swi.val)
		{
			prf(" ; [");
			prf2("<span class=sys>");
			prf("\"%s\"", syscall_desc[j].str);
			prf2("</span>");
			prf("]");
			if (inpr)
				pr[numpr].sys = j + 1;
			break;
		}
	}
}


/// Decode a data processing instruction. Note that for the most
/// part we use DPI1 until we need to worry about specifics.
void decode_dpi(uint32_t i)
{
	INST inst;
	inst.raw = i;

#ifdef DEBUG
	prf("\ncond   %x\n", inst.dpi1.cond);
	prf("pad    %x\n", inst.dpi1.pad);
	prf("hash   %x\n", inst.dpi1.hash);
	prf("opcode %x\n", inst.dpi1.opcode);
	prf("set    %x\n", inst.dpi1.set);
	prf("rn     %x\n", inst.dpi1.rn);
	prf("rd     %x\n", inst.dpi1.rd);
	prf("rot    %x\n", inst.dpi1.rot);
	prf("imm    %x\n\n", inst.dpi1.imm);
#endif

	prf2("<span class=inst>");
	prf("%s%s", dpiops[inst.dpi1.opcode], cond[inst.dpi1.cond]);

	if ((inst.dpi1.opcode >> 2) != 0x2)   
	    if (inst.dpi1.set != 0)
			prf("s");

	prf2("</span>");
	prf("\t");

	if ((inst.dpi1.opcode != 0xA) && (inst.dpi1.opcode != 0xB)
		&& (inst.dpi1.opcode != 0x8) && (inst.dpi1.opcode != 0x9))
	{
		prf("%s, ", regs[inst.dpi1.rd]);
		setregs[inst.dpi1.rd] = false;	// can't process only fixed data
	}
  
	if ((inst.dpi1.opcode != 0xD) && (inst.dpi1.opcode != 0xF))
		prf("%s, ", regs[inst.dpi1.rn]);

	if (inst.dpi1.hash != 0)
	{
		uint32_t t = inst.dpi1.imm >> (inst.dpi1.rot * 2);
		t |= (inst.dpi1.imm << (32 - (inst.dpi1.rot * 2)));

		prf("#%d\t", t);
		prf2("<span class=comm>");
		prf("; 0x%x", t); 
		if (t > 32 && t < 256)
			prf(" '%c'", t);
		prf2("</span>");
	}
	else
	{
		if (inst.dpi2.pad2 == 0)
		{
			prf("%s", regs[inst.dpi2.rm]);
			if (inst.dpi2.shift != 0)
			{
				prf(", %s #%d\t", shift[inst.dpi2.type], 
					inst.dpi2.shift);
				prf2("<span class=comm>");
				prf("; 0x%x", inst.dpi2.shift);
				prf2("</span>");
			}
		}
		else
		{
			prf("%s", regs[inst.dpi3.rm]);
			prf(", %s %s", shift[inst.dpi3.type], regs[inst.dpi3.rs]);
		}
	}

	if (inst.dpi1.opcode == 0xd && inst.dpi1.rd == 0xf)
	{
		prf(" ");
		prf2("<span class=comm>");
		prf("; %s", inst.dpi2.rm == 0xe ? "return" : "jump");
		if (setregs[inst.dpi2.rm])
		{
#ifndef NOADDR
			prf(" to ");
			prf2("<a class=link href=\"#%06x\">", armregs[inst.dpi2.rm]);
			prf("%06x", armregs[inst.dpi2.rm]);
			prf2("</a>");
#endif
		}
		if (inpr)
		{
			if (setregs[inst.dpi2.rm])
				pr[numpr].branch = armregs[inst.dpi2.rm];
			else if (inst.dpi1.cond == 0xe)
			{
				if (inst.dpi2.rm != 0xe)	// not mov pc, lr
				prf(" to ???");
				pr[numpr].branch = -1;	// don't do unknown jump, just return
			}
		}

		prf2("</span>");

		if (inpr)
		{
			prf("\n");
			if (inst.dpi1.cond == 0xe && inst.dpi2.rm == 0xe)
			{
				if (html)
					prf2("<hr>");
				else
					prf("\n");
			}
		}
	}
}


/// Decodes multiply instruction
void decode_mult(uint32_t i)
{
	INST inst;
	inst.raw = i;

	prf2("<span class=inst>");
	prf("%s%s", mult[inst.mult.opcode], cond[inst.mult.cond]);

	if (inst.mult.set != 0)
		prf("s");
	prf2("</span>");

	switch (inst.mult.opcode)
    {
	case MUL:
		prf("\t%s, %s, %s", regs[inst.mult.rd], regs[inst.mult.rm], 
			regs[inst.mult.rs]);
		break;
	case MLA:
		prf("\t%s, %s, %s, %s", regs[inst.mult.rd], regs[inst.mult.rm], 
			regs[inst.mult.rs], regs[inst.mult.rn]);
		break;
	case UMULL: case UMLAL: case SMULL: case SMLAL:
		prf("\t%s, %s, %s, %s", regs[inst.mult.rd], regs[inst.mult.rn],
			regs[inst.mult.rm], regs[inst.mult.rs]);
		break;
	default:
		prf("\t????");
		break;
    }
}


/// Decodes a single word transfer instruction.
void decode_swt(uint32_t i, uint32_t offs)
{
	INST inst;
	inst.raw = i;

	pr[numpr].sys = 0;

	// print the instruction name
	prf2("<span class=inst>");
	if (inst.swt1.ls != 0)
	{
		prf("ldr%s", cond[inst.swt1.cond]);
		setregs[inst.swt1.rd] = false;
	}
	else
		prf("str%s", cond[inst.swt1.cond]);

	if (inst.swt1.b != 0)
		prf("b");
	prf2("</span>");

	// The T bit???? 

	prf("\t%s, [%s", regs[inst.swt1.rd], regs[inst.swt1.rn]);
	if (inst.swt1.p == 0)
		prf("], ");
	else
		prf(", ");
  
	if (inst.swt1.hash == 0)
    {
		if (inst.swt1.u == 0)
			prf("-");
		prf("#%d", inst.swt1.imm);
	}
	else
	{
		prf("%s, %s #%x", regs[inst.swt2.rm], shift[inst.swt2.type],
			inst.swt2.shift);
	}
	if (inst.swt1.p == 1)
	{
		prf("]");
		if (inst.swt1.wb == 1)
			prf("!");
	}

	if (inst.swt1.hash == 0 && (inst.swt1.rn == 0xf || setregs[inst.swt1.rn] || (!inpr && pr[offs/4].sys == -3)))
	{
		int of = inst.swt1.rn == 0xf ? offs + 8 : (setregs[inst.swt1.rn] ? armregs[inst.swt1.rn] : pr[offs/4].branch);
		if (of >= 0)
		{
			int n = of + (inst.swt1.u == 0 ? -(int)inst.swt1.imm : inst.swt1.imm);
			if (n >= JASPER_SYSCTRL_BASE)
			{
				for (int k = 0; ports_desc[k].num != -1; k++)
				{
					if ((n & ports_desc[k].num) == ports_desc[k].num)
					{
#ifndef NOADDR
						prf(" ");
						prf2("<span class=comm>");
						prf("; [");
						prf2("<span class=sys>");
						prf("P_%s+%02x", ports_desc[k].str, n);
						prf2("</span>");
						prf("]");
						prf2("</span>");
#endif
						break;
					}
				}
			}
			else
			{ 
#ifndef NOADDR
				bool datalink = false;
				if (n >= (int)data_start && n <= (int)filelen && (FLT | ELF))
				{
					datalink = true;
				}

				reloc_pair *rpair = reloc_hash.Get(reloc_pair(0, n));
				char *nn = NULL;
				if (rpair != NULL)
				{
					if (rpair->name != NULL && rpair->name[0] != '\0')
						nn = rpair->name;
				}
				
				prf(" ");
				prf2("<span class=comm>");
				prf("; [");
				if (nn != NULL)
					prf("%06x, ", n);
				prf2("<a class=%s href=\"#%06x\">", (n >= 0 && n < (int)filelen) ? "dlink" : "sys",
					datalink ? (n & 0xfffffff0) : n);
				if (nn == NULL)
					prf("%06x", n);
				else
				{
					prf(html ? "&lt;" : "<");
					prf("%s", nn);
					prf(html ? "&gt;" : ">");
				}
				prf2("</a>");
				prf("]");

				// try to display data if it's text
				if (rpair != NULL && (FLT | (ELF && rpair->type == 2)))
				{
					if (rpair->addr >= (int)data_start && rpair->addr <= (int)filelen)
					{
						BYTE *cc = buf + rpair->addr;
						int len = 0;
						while (cc[len] >= 32 && cc[len] < 128 && len < 256)
							len++;
						if ((cc[len] == '\0' || cc[len] == 10) && len > 0)
						{
							strncpy(strtmp, (char *)cc, len+1);
							strtmp[len] = 0;
							prf("\t\"%s\"", strtmp);
						}
					}
				}

				prf2("</span>"); 
#endif
			}
			link_hash.Add(n);

			if (n >= 0 && n < (int)filelen)
			{
				if (inst.swt1.ls != 0)	// load reg from const. address
				{
					/*if (inst.swt1.rd == 0xc)
						armregs[inst.swt1.rd] = n;
					else*/
					{
						BYTE *curbuf = buf + n;
						DWORD dw = ReadDwordNet(curbuf);
						reloc_pair *rpair = reloc_hash.Get(reloc_pair(0, n));
						if (rpair != NULL)
						{
							if (FLT)
								dw += 0x40;
							//if (rpair->name != NULL)

						}
						armregs[inst.swt1.rd] = dw;
					}
					setregs[inst.swt1.rd] = true;
				}
			} else
				if (inpr)
				{
					pr[numpr].sys = -3; // just remember the addr. value for data
					pr[numpr].branch = n;
				}
		}
	}

	if (inst.swt1.ls != 0 && inst.swt1.rd == 0xf)
	{
		prf(" ");
		prf2("<span class=comm>");
		prf("; jump");
		if (setregs[inst.swt1.rn])
		{
#ifndef NOADDR
			prf(" to ");
			prf2("<a class=link href=\"#%06x\">",  armregs[inst.swt1.rn]);
			prf("%06x", armregs[inst.swt1.rn]);
			prf2("</a>");
#endif
		}
		if (inpr && inst.swt1.cond == 0xe)
		{
			if (setregs[inst.swt1.rn])
				pr[numpr].branch = armregs[inst.swt1.rn];
			pr[numpr].branch = -1;	// don't do jump, just return
		}
		prf2("</span>");
		if (inpr)
			prf("\n");
	}
}


/// Decodes a half word transfer.
void decode_hwt(uint32_t i)
{
	INST inst;
	inst.raw = i;

	// print the instruction name
	prf2("<span class=inst>");
	if (inst.hwt.ls == 1)
		prf("ldr%s", cond[inst.hwt.cond]);
	else
		prf("str%s", cond[inst.hwt.cond]);
  
	if (inst.hwt.s == 1)
	    prf("s");
	if (inst.hwt.h == 1)
		prf("h");
	else
		prf("b");
	prf2("</span>");

	prf("\t%s, [%s", regs[inst.hwt.rd], regs[inst.hwt.rn]);
	if (inst.hwt.p == 0)
		prf("], ");
	else
		prf(", ");

	if (inst.hwt.u == 0)
		prf("-");
  
	if (inst.hwt.hash == 1)
	{
		prf("#%d", (inst.hwt.imm << 4) + inst.hwt.rm);
	}
	else
	{
		prf("%s", regs[inst.hwt.rm]);
	}

	if (inst.hwt.p == 1)
    {
		prf("]");
		if (inst.hwt.wb == 1)
			prf("!");
	}
}


/// Decodes the Multiple Register Transfer instructions.
void decode_mrt(uint32_t i)
{
	uint32_t rlist, cur;

	INST inst;
	inst.raw = i;

	// print the instruction name
	prf2("<span class=inst>");
	if (inst.mrt.ls == 1)
		prf("ldm%s", cond[inst.mrt.cond]);
	else
		prf("stm%s", cond[inst.mrt.cond]);

	if (inst.mrt.u == 0)
		prf("d");
	else
		prf("i");

	if (inst.mrt.p == 0)
		prf("a"); 
	else
		prf("b");
	prf2("</span>");

	// base register
	prf("\t%s", regs[inst.mrt.rn]);
	if (inst.mrt.wb == 1)
		prf("!");

	prf(", {");
	rlist = inst.mrt.list;
	cur = 0;
	bool ret = false;
	while (rlist != 0)
    {
		if ((rlist & 0x1) == 0x1)
		{
			prf("%s", regs[cur]);
			if (cur == 0xf)
				ret = true;
			if (rlist > 1)
				prf(", ");
		}
		cur++;
		rlist = rlist >> 1;
	}

	prf("}");

	if (inst.mrt.ls == 1 && (inst.mrt.rn == 0xb || inst.mrt.rn == 0xd) && ret)
	{
		prf(" ");
		prf2("<span class=comm>");
		prf("; return");
		prf2("</span>");
		if (inst.mrt.cond == 0xe)
		{
			if (inpr)
			{
				prf("\n");
				if (html)
					prf2("<hr>");
				else
					prf("\n");
				pr[numpr].branch = -1;
			}
		}
	}
}


/// Decodes a swap instruction.
void decode_swp(uint32_t i)
{
	INST inst;
	inst.raw = i;

	prf2("<span class=inst>");
	prf("swp%s", cond[inst.swap.cond]);

	if (inst.swap.byte == 1)
		prf("b");
	prf2("</span>");

	prf("\t%s, %s, [%s]", regs[inst.swap.rd], regs[inst.swap.rm],
		regs[inst.swap.rn]);
}


/// Decodes a status to general register transfer.
void decode_sgr(uint32_t i)
{
	INST inst;
	inst.raw = i;

	prf2("<span class=inst>");
	prf("msr%s", cond[inst.mrs.cond]);
	prf2("</span>");
	prf("\t%s, ", regs[inst.mrs.rd]);

	if (inst.mrs.which == 1)
		prf(" cpsr");
	else
		prf(" spsr");
}


/// Decodes a general to status register transfer.
void decode_gsr(uint32_t i)
{
	INST inst;
	inst.raw = i;

	prf2("<span class=inst>");
	prf("mrs%s", cond[inst.msr1.cond]);
	prf2("</span>");
	prf("\t");

	if (inst.msr1.which == 1)
		prf(" cpsr_");
	else
		prf(" spsr_");

	if (inst.msr1.hash == 1)
	{
		prf("f, #%x", inst.msr1.imm << inst.msr1.rot);
	}
	else
	{
		if (inst.msr2.field & 0x1)
			prf("c, ");
		else if (inst.msr2.field & 0x2)
			prf("x, ");
		else if (inst.msr2.field & 0x4)
			prf("s, ");
		else
			prf("f, ");

		prf("%s", regs[inst.msr2.rm]);
	}
}


/// Decodes a CoPro data op.
void decode_cdo(uint32_t i)
{
	INST inst;
	inst.raw = i;

	prf2("<span class=inst>");
	prf("cdp%s", cond[inst.cdo.cond]);
	prf2("</span>");
	prf("\t");

	prf("p%x, %x, %s, %s, %s", inst.cdo.cpn, inst.cdo.cop1, 
		cregs[inst.cdo.crd], cregs[inst.cdo.crn], cregs[inst.cdo.crm]);

	if (inst.cdo.cop2 != 0)
		prf(", %x", inst.cdo.cop2);
}


/// Decodes a CoPro data transfer.
void decode_cdt(uint32_t i)
{
	INST inst;
	inst.raw = i;

#ifdef DEBUG
	prf("\ncond   %8x\n", inst.cdt.cond);
	prf("pad    %8x\n", inst.cdt.pad);
	prf("p      %8x\n", inst.cdt.p);
	prf("u      %8x\n", inst.cdt.u);
	prf("n      %8x\n", inst.cdt.n);
	prf("wb     %8x\n", inst.cdt.wb);
	prf("ls     %8x\n", inst.cdt.ls);
	prf("rn     %8x\n", inst.cdt.rn);
	prf("crd    %8x\n", inst.cdt.crd);
	prf("cpn    %8x\n", inst.cdt.cpn);
	prf("offset %8x\n\n", inst.cdt.offset);
#endif

	prf2("<span class=inst>");
	if (inst.cdt.ls == 1)
		prf("ldc%s", cond[inst.cdt.cond]);
	else
		prf("stc%s", cond[inst.cdt.cond]);

	if (inst.cdt.n == 1)
		prf("l");
	prf2("</span>");

	prf("\tp%x, %s, [%s", inst.cdt.cpn, cregs[inst.cdt.crd], 
		regs[inst.cdt.rn]);

	if (inst.cdt.p == 0)
		prf("]");

	if (inst.cdt.offset != 0)
		prf(", #%d", inst.cdt.offset);
  
	if (inst.swt1.p == 1)
	{
		prf("]");
		if (inst.swt1.wb == 1)
			prf("!");
	}
}


/// Decode a CoPro register transfer.
void decode_crt(uint32_t i)
{
	INST inst;
	inst.raw = i;

	prf2("<span class=inst>");
	if (inst.crt.ls == 1)
		prf("mcr");
	else
		prf("mrc");

	prf("%s", cond[inst.crt.cond]);
	prf2("</span>");
	prf("\t p%x, %x, %s, %s, %s", inst.crt.cpn,
		inst.crt.cop1, regs[inst.crt.rd], cregs[inst.crt.crn], 
		cregs[inst.crt.crm]);

	if (inst.crt.cop2 != 0)
		prf(", %x", inst.crt.cop2);
}


/// Decides what type of instruction this word is and passes it 
/// on to the appropriate child decoder. Note that the ordering 
/// here is important. E.g. you need to test for a mutliply 
/// instruction *before* a DPI instruction - a mult looks like
/// an AND too. :-(
int decode_word(uint32_t i, uint32_t offs)
{
#ifndef NOADDR
	prf2("<span class=raw>");
	prf("%08x", i); 
	prf2("</span>");
#else
	if ((i & 0xfff00000) == 0x01000000)
	{
		prf("\t***");
		return 0;
	}
#endif
	
	if (i == 0 || i == 0xe1a00000)
	{
#ifndef NOADDR
		prf("\t");
		prf2("<span class=inst>");
		prf("nop");
		prf2("</span>");
#endif
		return -1;
	}
	prf("\t");
  
	if ((i & BRANCH_MASK) == BRANCH_SIG)
    {
		decode_branch(i, offs);
    }
	else if ((i & SWI_MASK) == SWI_SIG)
    {
		decode_swi(i);
    }
	else if ((i & MULT_MASK) == MULT_SIG)
    {
		decode_mult(i);
    }
	else if ((i & SWT_MASK) == SWT_SIG)
    {
		decode_swt(i, offs);
    }
	else if ((i & HWT_MASK) == HWT_SIG)
    {
		decode_hwt(i);
    }
	else if ((i & DPI_MASK) == DPI_SIG)
    {
		decode_dpi(i);
    }
	else if ((i & MRT_MASK) == MRT_SIG)
    {
		decode_mrt(i);
    }
	else if ((i & SWP_MASK) == SWP_SIG)
    {
		decode_swp(i);
    }
	else if ((i & MRS_MASK) == MRS_SIG)
    {
		decode_sgr(i);
    }
	else if ((i & MSR_MASK) == MSR_SIG)
    {
		decode_gsr(i);
    }
	else if ((i & CDO_MASK) == CDO_SIG)
    {
		decode_cdo(i);
    }
	else if ((i & CDT_MASK) == CDT_SIG)
    {
		decode_cdt(i);
    }
	else if ((i & CRT_MASK) == CRT_SIG)
    {
		decode_crt(i);
    }
	else if ((i & UAI_MASK) == UAI_SIG)
    {
		prf("Unused arithmetic op");
    }
	else if ((i & UCI1_MASK) == UCI1_SIG)
    {
		prf("Unused control 1");
    }
	else if ((i & UCI2_MASK) == UCI2_SIG)
    {
		prf("Unused control 2");
    }
	else if ((i & UCI3_MASK) == UCI3_SIG)
    {
		prf("Unused control 3");
    }
	else if ((i & ULSI_MASK) == ULSI_SIG)
    {
		prf("Unused load/store");
    }
	else if ((i & UCPI_MASK) == UCPI_SIG)
    {
		prf("Unused CoPro");
    }
	else if ((i & UNDEF_MASK) == UNDEF_SIG)
    {
		prf("Undefined");
    }
	else
    {
		prf("Rubbish");
    }
	return 0;
}

//////////////////////////////////////////////////////////////////////

int pr_decode(uint32_t data, int i, int shift = 0, int condshift = 0)
{
	for (int j = 0; j < shift; j++)
		prf(".\t");
	if (shift > MAXSHIFT || condshift > MAXCONDSHIFT)
	{
		prf("TOO DEEP.\n");
		return 0;
	}
	reloc_pair *rpair = reloc_hash.Get(reloc_pair(0, i));
	if (rpair != NULL && (FLT | (ELF && rpair->type == 2)))
	{
		uint32_t temp = data;
		if (FLT)
		{
			temp  = (data & 0x000000FF) << 24;
			temp |= (data & 0x0000FF00) << 8;
			temp |= (data & 0x00FF0000) >> 8;
			temp |= (data & 0xFF000000) >> 24;
		}

		prf2("<span class=raw>");
		prf("%08x", temp);
		prf2("</span>");
		prf("---------------------> ");

		if (rpair->addr > (int)filelen)
		{
#ifndef NOADDR		
			prf("%08x\t ", rpair->addr);
#endif
			prf2("<span class=comm>");
			prf("; !Unknown!");
			prf2("</span>");
		}
		else
		{
			char *nn = NULL;
			if (rpair->name != NULL && rpair->name[0] != '\0')
				nn = rpair->name;
#ifndef NOADDR		
			if (nn != NULL)
				prf("%06x, ", rpair->addr);
#endif
			prf2("<a class=link href=\"#%06x\">", 
				(rpair->addr >= (int)data_start) ? (rpair->addr & 0xfffffff0) : rpair->addr);

#ifndef NOADDR					
			if (nn == NULL)
				prf("%06x", rpair->addr);
			else
#endif
			{
				prf(html ? "&lt;" : "<");
				prf("%s", nn);
				prf(html ? "&gt;" : ">");
			} 
			prf2("</a>");
			link_hash.Add(rpair->addr);
			if (rpair->addr >= (int)data_start)
			{
				prf(" ");
				prf2("<span class=comm>");
				prf("; data:");
				prf2("</span>");
				prf("\n\t\t");
				BYTE *cc = buf + rpair->addr;
				pr_data(cc);
			} else
			{
				prf(" ");
				prf2("<span class=comm>");
				prf("; code");
				prf2("</span>");
				prf("\n\t\t");
			}
		}
		if (inpr)
		{
			pr[numpr].flag = 1;	// data
			pr[numpr].branch = -1;
		}
	  
	} else
		return decode_word(data, i);
	return 0;
}

void pr_label(int i)
{
	prf2("<span class=label>");
	/*if (ELF)
		i -= starti;
	*/
	prf("%06x", i);
	prf2("</span>");
	prf(":  ");
}

void pr_callgraph(int i, int shift = 0, int condshift = 0)
{
	int start0 = i, starti = i;
	int excl = exclude.GetN();
	
	if (i < 0 || i >= (int)filelen)
	{
		pr_label(i);
		prf("ERROR - out of range.\n");
		return;
	}
	pr[i/4].flag = 2;
	if (shift > MAXSHIFT || condshift > MAXCONDSHIFT)
	{
		pr_label(i);
		pr_decode(*(DWORD *)(buf+i), i, shift, condshift);
		return;
	}

	if (pr[i/4].cnt > MAXREPEAT)
	{
		pr_label(i);
		for (int j = 0; j < shift; j++)
			prf(".\t");
		prf("<see above>\n");
		return;
	}
	

	for (int k = 0; k < (int)exclude.GetN(); k++)
	{
		if (i >= exclude[k].i1 && i <= exclude[k].i2 )	// already were here
			return;
	}


	/*pr_label(i);
	pr_decode(*(DWORD *)(buf+i), i, shift);
	prf("\n");
	*/

	bool wasany = false;
	bool interesting = false;

	RDList<int> calls;

	for ( ; i < (int)flth.data_start; i += 4)
	{
		pr[i/4].flag = 2;
		if (pr[i/4].branch == -1)
		{
			break;
		}
		if (pr[i/4].branch != 0 && pr[i/4].sys != -3)
		{
			wasany = true;
			
			if (pr[i/4].sys == 0)		// goto
			{
				/*pr_label(i);pr_decode(*(DWORD *)(buf+i), i, shift);
				prf("\n");*/
				if (pr[i/4].branch == i)
					break;
				if (pr[i/4].branch < starti || pr[i/4].branch > i)
				{
					// also check other excludes
					bool fl = true;
					for (int k = 0; k < (int)exclude.GetN(); k++)
					{
						if (pr[i/4].branch >= exclude[k].i1 && pr[i/4].branch <= exclude[k].i2)	// already were here
							fl = false;
					}
					if (fl)
					{
						exclude.Merge(range(starti, i));	// add current range and wait for next..
						i = pr[i/4].branch - 4;
						starti = i+4;
						if (i < 0 || i+4 >= (int)filelen)
						{
							prf("ERROR - out of range.\n");
							break;
						}
						continue;
					}
				}

			} else			// call
			{
				wasany = true;
				// find 'return' point first
				/*for (int j = i ; j < (int)flth.data_start; j += 4)
				{
					if (pr[j/4].sys == 1 || (pr[j/4].sys == 0 && (pr[j/4].branch < starti || pr[j/4].branch > i)) || pr[j/4].branch == -1)
						break;
				}
				if (pr[i/4].sys == -2)// conditional
				*/
				
				calls.Add(i);
				/*else
				{
					exclude.Merge(range(starti, j));
					pr_callgraph(pr[i/4].branch, shift + 1, condshift);
					exclude.Remove(range(starti, j));
				}*/
			}
		} 
		else if (pr[i/4].sys != 0)
		{
			calls.Add(i);
			if (pr[i/4].sys == 1)		// "exit"
				break;
			else
				interesting = true;
		}
	}
	exclude.Merge(range(starti, i));

	// now make calls
	DWORD l;
	for (l = 0; l < calls.GetN(); l++)
	{
		if ((pr[calls[l]/4].branch == 0 || pr[calls[l]/4].sys == -3) && pr[calls[l]/4].sys != 0)
		{
			pr_label(calls[l]);pr_decode(*(DWORD *)(buf+calls[l]), calls[l], shift);
			prf("\n");
		} else
		{
			pr_label(calls[l]);pr_decode(*(DWORD *)(buf+calls[l]), calls[l], shift);
			prf("\n");
			pr_callgraph(pr[calls[l]/4].branch, (pr[calls[l]/4].sys == -2) ? shift : shift + 1, condshift);
		}
	}

	if (!wasany)
	{
		pr_label(i);
		for (int j = 0; j < shift; j++)
			prf(".\t");
		prf("return\n");
	}

	// restore exclude list
	DWORD nn = exclude.GetN();
	for (l = excl; l < nn; l++)
		exclude.Remove(excl);
	if (!interesting)
		pr[start0/4].cnt++;
}

//////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	FILE* fp;
	uint32_t data;
	uint32_t i;

	char *ss[] =
	{
		"ARM2HTML. FLT/ELF/RAW Disassembler...\n",
		"Version 0.1g, (c) bombur, 2004-2007. Questions: bombur@ukrpost.net\n",
		"Freeware. No warranties or support provided! Use it on your own risk!\n\n",
	};
  
	fprintf(stderr, "%s%s%s", ss[0], ss[1], ss[2]);

	// Did we get the correct number of parameters?
	if (argc < 2)
	{
		fprintf(stderr, "Usage: " 
#ifndef NOADDR
			"ARM2HTML"
#else
			"ARM2HTML-NOADDR"
#endif
			" [-h] [-d] [-c<OFFSET>] binary > output_asm.html\n");
		fprintf(stderr, "    -h = html output;\n");
		fprintf(stderr, "    -d = data section output;\n");
		fprintf(stderr, "    -c = code graph output, starting from offset (experimental).\n\n");
		exit(EXIT_FAILURE);
	}

	bool putdata = false, putcalls = false;
	char *fname = argv[1];
	int startfrom = -1;
	if (argc >= 3 && argc <= 5)
	{
		for (int k = 1; k < argc; k++)
		{
			if (argv[k][0] == '-')
			{
				if (tolower(argv[k][1]) == 'd')
				{
					putdata = true;
				}
				else if (tolower(argv[k][1]) == 'h')
				{
					html = true;
				}
				else if (tolower(argv[k][1]) == 'c')
				{
					putcalls = true;
					if (argv[k][2] != '\0')
						startfrom = atoi(argv[k]+2);
				}
			} else
				fname = argv[k];
		}
	}
	// try opening the file 
	if ((fp = fopen(fname, "rb")) == NULL)
    {
		fprintf(stderr, "Failed to open file %s\n", fname);
		exit(EXIT_FAILURE);
    }

	fseek(fp, 0, SEEK_END);
	filelen = ftell(fp);
	rewind(fp);

	buf = (BYTE *)malloc(filelen);
	curbuf = buf;
	fread(buf, filelen, 1, fp);

	if (strncmp((char *)buf, "bFLT", 4) == 0)
	{
		FLT = true;
	} else if (strncmp((char *)buf, "\177ELF", 4) == 0)
	{
		ELF = true;
	}
	
	if (FLT)
	{
		memcpy(flth.magic, curbuf, 4); 
		curbuf += 4;
		flth.rev = ReadDwordNet(curbuf);
		flth.entry = ReadDwordNet(curbuf);
		flth.data_start = ReadDwordNet(curbuf);
		flth.data_end = ReadDwordNet(curbuf);
		flth.bss_end = ReadDwordNet(curbuf);
		flth.stack_size = ReadDwordNet(curbuf);
		flth.reloc_start = ReadDwordNet(curbuf);
		flth.reloc_count = ReadDwordNet(curbuf);
		flth.flags = ReadDwordNet(curbuf);
		//h.filler
	}

	if (ELF)
	{
		memcpy(&elfh, curbuf, sizeof(elfh));
	}

	if (html)printf("<html>\n<head><title>");
	printf("ARM disassembly :: %s ", fname);
	if (html)printf("</title>");
	printf("\n");
	char *style = "BODY { FONT-SIZE: 12px; Background-Color: #bcbcac; COLOR: #333333; FONT-FAMILY: Terminal, Arial }\n"
			".label { COLOR: #7f007f; font-weight:bold }\n"
			".inst { COLOR: #0000cf; font-weight:normal }\n"
			".raw { COLOR: #007070; }\n"
			".comm { COLOR: #007070; }\n"
			".sys { COLOR: #7f007f; font-weight:bold }\n"
			".funcname { COLOR: #951010; font-weight:bold }\n"
			".link { COLOR: #a0373f; TEXT-DECORATION: underline }\n"
			".link:hover { COLOR: red }\n"
			".dlink { COLOR: #502f0f; TEXT-DECORATION: underline }\n"
			".dlink:hover { COLOR: red }\n"
			".data { COLOR: #303030; FONT-SIZE: 11px; FONT-FAMILY:Courier New; }";
	FILE *ff = fopen("arm2html.css", "rt");
	if (ff != NULL)
	{
		fseek(ff, 0, SEEK_END);
		int sl = ftell(ff);
		rewind(ff);
		style = (char *)RDmalloc(sl + 1);
		fread(style, sl, 1, ff);
		fclose(ff);
		style[sl] = '\0';
	}
	if (html)
		printf("<STYLE type=text/css>\n%s</STYLE>\n</head>\n<body>\n<pre>\n", style);
	numpr = 0;
	pr = (record *)RDmalloc(filelen/4 * sizeof(record));
	memset(pr, 0, filelen/4 * sizeof(record));

	printf("%s%s%s", ss[0], ss[1], ss[2]);

	if (html)
	{
		printf("<a href=\"#CodeSection\">Code Section</a>\t");
		if (FLT && putdata)
			printf("<a href=\"#DataSection\">Data Section</a>\t");
		if (FLT && putcalls)
			printf("<a href=\"#CallGraph\">Call Graph</a>");
		printf("\n<br>");
	}
	printf("\n");

	fprintf(stderr, "Processing:\n  Code section...");

	memset(armregs, 0, 16 * sizeof(DWORD));
	memset(setregs, 0, 16 * sizeof(bool));

	RDList<int> len;

	if (FLT)
	{
		printf("BINFLT file format. Fileflags: ");
		if ((flth.flags & FLAT_FLAG_RAM) == FLAT_FLAG_RAM)
			printf("RAM ");
		if ((flth.flags & FLAT_FLAG_GOTPIC) == FLAT_FLAG_GOTPIC)
			printf("GOTPIC ");
		if ((flth.flags & FLAT_FLAG_GZIP) == FLAT_FLAG_GZIP)
			printf("GZIP ");
		printf("\n\n");

		if (flth.rev != 4)
		{
			printf("Wrong file version. Should be BFLT4!\n");
			return 3;                                 
		}

		if ((flth.flags & FLAT_FLAG_GZIP) == FLAT_FLAG_GZIP)
		{
			/*
			int newsize = h.reloc_start + h.reloc_count * 4;
			BYTE *oldbuf = buf;
			buf = (BYTE *)malloc(newsize + 10);
			memcpy(buf, oldbuf, sizeof(h));
			free(oldbuf);
			
			gzFile gzf = gzdopen((int)fp, "rb");

			int dstlen = newsize - sizeof(h);
			int ret = gzread (gzf, buf + sizeof(h), dstlen);
			if (ret != dstlen)
			{
				printf("Gzip decompression error.\n");
				return;
			}
			filelen = newsize;
			gzclose(gzf);
			*/
			printf("ZFLAT not supported.\n");exit(1);
		} 
		/*else
			fread(buf + sizeof(h), filelen - sizeof(h), 1, fp);
		*/


		fclose(fp);

		//printf("Entry: 0x%08x, flags: 0x%08x\n\n", h.entry, h.flags);

		bool totalfound = false;
		int numfound = 0;

		curbuf = buf + flth.reloc_start;

		DWORD *addrs = new DWORD [flth.reloc_count];

		reloc_hash.SetN(flth.reloc_count);
		link_hash.SetN(flth.reloc_count);		// not quite correct!

		for (i = 0; i < (int)flth.reloc_count; i++)
		{
			DWORD offs = ReadDwordNet(curbuf) + sizeof(flth);
			BYTE *oldo = curbuf;
			curbuf = buf + offs;
			DWORD addr = 0;
			addr = ReadDwordNet(curbuf) + 0x40;
			addrs[i] = addr;
			reloc_hash.Merge(reloc_pair(addr, offs));
			curbuf = oldo;
		}

		data_start = flth.data_start;
	} 
	else if (ELF)
	{
		if (elfh.e_machine != 40)	// arm
		{
			printf("Wrong arch. type. Only ARM binaries supported.");
			return 4;
		}
		// TODO: add more checks
		
		if (elfh.e_shoff > 0)
		{
			if (elfh.e_shentsize != sizeof(Elf32_Shdr))
			{
				printf("Wrong section format");
				return 5;
			}
			
			Elf32_Shdr *shdr = (Elf32_Shdr *)(buf + elfh.e_shoff);
			DWORD stroffs = shdr[elfh.e_shstrndx].sh_offset;
			int i, symtab = 0, strtab = 0, reloc = -1;
			RDList<int> text;
			for (i = 1; i < elfh.e_shnum; i++)
			{
				char *sname = (char *)buf + stroffs + shdr[i].sh_name;
				if (strcmp(sname, ".text") == 0 || strncmp(sname, ".gnu.linkonce", 13) == 0)
				{
					starti.Add(shdr[i].sh_offset);
					len.Add(shdr[i].sh_offset + shdr[i].sh_size + 1);
					text.Add(i);
				}
				if (strcmp(sname, ".symtab") == 0)
				{
					symtab = i;
				}

				if (strcmp(sname, ".strtab") == 0)
				{
					strtab = i;
				}

				if (strcmp(sname, ".rel.text") == 0)
				{
					reloc = i;
				}

				if (strcmp(sname, ".rodata") == 0)
				{
					data_start = shdr[i].sh_offset;
				}
			}


			int num = shdr[symtab].sh_size / sizeof(Elf32_Sym);
			Elf32_Sym *s = (Elf32_Sym *)(buf + shdr[symtab].sh_offset);
			for (i = 1; i < num; i++)
			{
				char *sname = (char *)buf + shdr[strtab].sh_offset + s[i].st_name;
				int type = ELF32_ST_TYPE(s[i].st_info);
				int bind = ELF32_ST_BIND(s[i].st_info);
				if (type == STT_FUNC || type == STT_OBJECT)
				{
					for (int U = 0; U < (int)starti.GetN(); U++)
					{
						if (s[i].st_shndx == text[U])
						{
							symb_hash.Merge(reloc_pair(0, starti[U] + s[i].st_value, sname));
						}
					}
				}
			}

			if (reloc < 0)
				printf("No relocations found...\n");
			else
			{
				if (shdr[reloc].sh_type != SHT_REL)
				{
					printf("Wrong reloc. format = %d", shdr[reloc].sh_type);
					return 6;
				}
				num = shdr[reloc].sh_size / sizeof(Elf32_Rel);
				Elf32_Rel *rel = (Elf32_Rel *)(buf + shdr[reloc].sh_offset);
				for (i = 1; i < num; i++)
				{
					int type = ELF32_R_TYPE(rel[i].r_info);
					int sym = ELF32_R_SYM(rel[i].r_info);
					char *sname = (char *)buf + shdr[strtab].sh_offset + s[sym].st_name;
					int base = 0;
					if ((short)s[sym].st_shndx > 0)
					{
						base = shdr[s[sym].st_shndx].sh_offset;
						if (s[sym].st_shndx > 1 && sname[0] == '\0')	// not '.text'
							sname = (char *)buf + stroffs + shdr[s[sym].st_shndx].sh_name;
					}
					/*
					int bind = ELF32_ST_BIND(s[sym].st_info);
					int stype = ELF32_ST_TYPE(s[sym].st_info);
					*/
					
					reloc_pair rp(s[sym].st_value, rel[i].r_offset + starti[0], sname, type);
					rp.CalcAddr(base);
					rp.symb = sym;
					reloc_hash.Merge(rp);
				}
			}
		}

	}
	else
	{
		/*read(buf + sizeof(h), filelen - sizeof(h), 1, fp); */
		printf("RAW file format.\n");
	}

	if (starti.GetN() == 0)
		starti.Add(0);
	if (len.GetN() == 0)
		len.Add(filelen);

	if (html)printf("<hr>\n<a name=CodeSection></a><h1>");
	else
		printf("\n");
	printf("CODE SECTION");
	if (html)
		printf("</h1>");
	printf("\n");

	curbuf = buf;
	if (FLT)
	{
		starti.Clear();
		starti.Add(flth.entry);
		len.Clear();
		len.Add(flth.data_start);
	}
	
	for (DWORD U = 0; U < starti.GetN(); U++)
	{

	curbuf = buf + starti[U];
	
	int idx = starti[U];
	for (i = starti[U]; i < (DWORD)len[U]; i += 4, curbuf += 4)
    {
		data = *(DWORD *)curbuf;

		pr_set(i/4);

		reloc_pair *p = symb_hash.Get(reloc_pair(0, i));

		if (p != NULL && p->name != NULL)
		{
			prf("\n");
			if (html)prf("<span class=funcname>");
			prf("%s%s%s", html ? "&lt;" : "<", p->name, html ? "&gt;" : ">");
			if (html)prf("</span>");
			prf("\n");
		}
		
#ifdef NOADDR
		if (data != 0 && data != 0xe1a00000) // skip nops
			prf("%06x ", idx);
		else
		{
			continue;
		}
#else
		pr_label(i);
#endif
		int ret;
		if (FLT || ELF)
		{
			ret = pr_decode(data, i);
		} else
		   ret = decode_word(data, i);
#ifdef NOADDR
		printf("%s\n", pr[i/4]);
#endif
		idx += 4;
    }
#ifndef NOADDR
	for (i = starti[U]/4; i < (DWORD)numpr; i++)
	{
		if (html)
		{
			if (link_hash.Get(i*4) != NULL || symb_hash.Get(reloc_pair(0, i*4)) != NULL)
				printf("<a name=%06x></a>", i*4);
		}
		printf("%s\n", pr[i]);
	}
#endif

	}

	if (FLT)
	{
		if (html)printf("<hr>\n<a name=CallGraph></a><h1>");
		else
			printf("\n");
		printf("CALL GRAPH");
		if (html)
			printf("</h1>");
		printf("\n");

		if (!putcalls)
		{
			if (html)printf("<i>");
			printf("--- use '-c' option to put call graph here ---");
			if (html)printf("</i>");
			printf("\n");
		}
		else
		{
			fprintf(stderr, "\n  Call graph...");
			inpr = false;
			prf("<-- Entry point -->\n");
			pr_callgraph(startfrom == -1 ? starti[0] : startfrom);
			prf("\n");
			if (html)
				prf2("<hr>");
			else
				prf("\n");
			// now get summary
			int tnum = flth.data_start/4 - flth.entry/4, t = 0;
			prf("Unvisited addresses:");
			int last = -1, ik = 0, ikk = 0;
			for (i = flth.entry/4; i < flth.data_start/4; i++)
			{
				if (pr[i].flag != 0)
					t++;
				else 
				{
					if ((int)i*4 != last*4 + 4)
					{
						if (ikk != 0)
							prf(" [%d bytes]", ikk);
						prf("\n");
						pr_label(i*4);
						ikk = 1;
					}
					last = i;
					if (ikk++ < 50)
						prf(".");

					ik++;
				}
			}
			if (ik == 0)
				prf(" none!");
			prf("\n");
			prf2("<b>");
			prf("(Accuracy of calls tracing: %d%s)\n\n", t * 100 / tnum, html ? "&#37;" : "%%");
			prf2("</b>");
			fprintf(stderr, "%d%% accuracy", t * 100 / tnum);

		}
		
		curbuf = buf + flth.data_start;
		
		
		if (html)printf("<hr>\n<a name=DataSection></a><h1>");
		else
			printf("\n");
		printf("DATA SECTION");
		if (html)
			printf("</h1>");
		printf("\n");

		if (!putdata)
		{
			if (html)printf("<i>");
			printf("--- use '-d' option to put data section here ---");
			if (html)printf("</i>");
			printf("\n");
		} else
		{
			fprintf(stderr, "\n  Data section...");
			for (i = flth.data_start/4; i < flth.data_end/4; i += 4)
			{
				if (html)
				{
					bool needed = false;
					for (int k = 0; k < 16; k++)
					{
						if (link_hash.Get(i*4 + k) != NULL)
						{
							needed = true;
							break;
						}
					}
					if (needed)
						printf("<a name=%06x></a>", i*4);
				}
				if (html)printf("<span class=label>");
				printf("%4x", i*4);
				if (html)printf("</span>");
				printf(":   ");
				if (html)printf("<span class=data>");
					int j;
				  for (j = 0; j < 16; j++)
					  printf("%02X ", curbuf[j]);
				  printf("| ");
				  for (j = 0; j < 16; j++)
				  {
					  if(curbuf[j] == 0x3c && html)
						  printf("&lt;");
					  else
						  printf("%c", strchr("\x1c\x1e\x0d\x1f\t\r\n", curbuf[j]) == NULL ? curbuf[j] : '.');
				  }
				  if (html)
					printf("</span>");
				  printf("\n");
				curbuf += 16;
			}
		}
	}
	if (html)
		printf("</pre></body></html>");
	fprintf(stderr, "\nDONE.\n");

	return 0;
}


void reloc_pair::CalcAddr(int startoff)
{
	int curval = (*(int *)(buf + offs));
	switch (type)
	{
	case 1:			/* R_ARM_PC24 */
		{
		int cv = (curval) & 0xffffff;
		if ((cv & 0x800000) == 0x800000)
			cv |= 0xff000000;
		cv += 2;
		if (addr != 0)
		{
			if (cv > 0)
			{
				int k = 4;
			}
			cv = 0;
		}
		addr += cv * 4 + startoff;
		break;
		}
	case 2:		/* R_ARM_ABS32 */
		addr += curval + startoff;
		break;
	case 0x1b:	/* R_ARM_PLT32 */
		addr += startoff;
		break;
	default:
		printf("Wrong reloc. type: %d", type);
		exit(7);
		break;
	}

	//(*(int *)(buf + offs) ) = (curval & 0xff000000) | (addr & 0x00ffffff);
}

