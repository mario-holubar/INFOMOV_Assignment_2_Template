#pragma once

namespace Tmpl8 {

//#define CACHE_TYPE		AssociativeCache
//#define CACHE_TYPE		DirectMappedCache
#define CACHE_TYPE		NWaySetAssociativeCache
#define CACHELINEWIDTH	64
#define LOG_N			6	// Logarithm of the number of entries in each set for the N-way set associative cache
#define L1_LOG_SIZE		6
#define L2_LOG_SIZE		10
#define L3_LOG_SIZE		12

#define L1_SIZE			(CACHELINEWIDTH << L1_LOG_SIZE)	// 2^6 lines @ 64 width => 4 KB
#define L2_SIZE			(CACHELINEWIDTH << L2_LOG_SIZE)	// 2^10 lines @ 64 width => 64 KB
#define L3_SIZE			(CACHELINEWIDTH << L3_LOG_SIZE)	// 2^12 lines @ 64 width => 256 KB
#define DRAM_SIZE		3276800							// 3.125 MB; 1024x800 pixels

struct CacheLine
{
	uchar bytes[CACHELINEWIDTH] = {};
	uint tag;
	bool dirty = false;
};

class Level // abstract base class for a level in the memory hierarchy
{
public:
	virtual void WriteLine( uint address, CacheLine line ) = 0;
	virtual CacheLine ReadLine( uint address ) = 0;
	Level* nextLevel = 0;
	uint r_hit = 0, r_miss = 0, w_hit = 0, w_miss = 0;
};

class AssociativeCache : public Level // associative cache
{
public:
	AssociativeCache(const uint cache_size)
	{
		size = cache_size;
		slot = new CacheLine[cache_size / CACHELINEWIDTH];
		memset(slot, 0, cache_size / CACHELINEWIDTH * sizeof(CacheLine));
	}
	void WriteLine( uint address, CacheLine line );
	void EvictLine( uint address, CacheLine line );
	CacheLine ReadLine( uint address );
	CacheLine& backdoor( int i ) { return slot[i]; } /* for visualization without side effects */
private:
	uint size;
	CacheLine* slot;
};

class DirectMappedCache : public Level // direct mapped cache
{
public:
	DirectMappedCache(const uint cache_size)
	{
		size = cache_size;
		slot = new CacheLine[cache_size / CACHELINEWIDTH];
		memset(slot, 0, cache_size / CACHELINEWIDTH * sizeof(CacheLine));
	}
	void WriteLine(uint address, CacheLine line);
	void EvictLine(uint address, CacheLine line);
	CacheLine ReadLine(uint address);
	CacheLine& backdoor(int i) { return slot[i]; } /* for visualization without side effects */
private:
	uint size;
	CacheLine* slot;
};

class NWaySetAssociativeCache : public Level // N-way set associative cache
{
public:
	NWaySetAssociativeCache(const uint cache_size)
	{
		size = cache_size;
		slot = new CacheLine[cache_size / CACHELINEWIDTH];
		memset(slot, 0, cache_size / CACHELINEWIDTH * sizeof(CacheLine));
	}
	void WriteLine(uint address, CacheLine line);
	void EvictLine(uint address, CacheLine line);
	CacheLine ReadLine(uint address);
	CacheLine& backdoor(int i) { return slot[i]; } /* for visualization without side effects */
private:
	uint size;
	CacheLine* slot;
};

class Memory : public Level // DRAM level for the memory hierarchy
{
public:
	Memory()
	{
		mem = new uchar[DRAM_SIZE];
		memset( mem, 0, DRAM_SIZE ); 
	}
	void WriteLine( uint address, CacheLine line );
	CacheLine ReadLine( uint address );
	uchar* backdoor() { return mem; } /* for visualization without side effects */
private:
	uchar* mem = 0;
};

class MemHierarchy // memory hierarchy
{
public:
	MemHierarchy()
	{
		l1 = new CACHE_TYPE(L1_SIZE);
		l1->nextLevel = l2 = new CACHE_TYPE(L2_SIZE);
		l2->nextLevel = l3 = new CACHE_TYPE(L3_SIZE);
		l3->nextLevel = dram = new Memory();
	}
	void WriteByte( uint address, uchar value );
	uchar ReadByte( uint address );
	void WriteUint( uint address, uint value );
	uint ReadUint( uint address );
	void ResetCounters()
	{
		l1->r_hit = l1->w_hit = l1->r_miss = l1->w_miss = 0;
		l2->r_hit = l2->w_hit = l2->r_miss = l2->w_miss = 0;
		l3->r_hit = l3->w_hit = l3->r_miss = l3->w_miss = 0;
		dram->r_hit = dram->w_hit = dram->r_miss = dram->w_miss = 0;
	}
	Level *l1, *l2, *l3, *dram;
};

} // namespace Tmpl8