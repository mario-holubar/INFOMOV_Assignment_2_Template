#include "precomp.h"
#include "game.h"
#include <iostream>

#define SPIRAL 0
#define FRACTAL 1
#define ANT 2
#define ALGORITHM ANT
#define BENCHMARK_STEPS 1000 // 10000 for spiral, 100 for fractal, 1000 for ant

// static variables for graph / fractal drawing / obfuscation
static float a = 0, r = 300;
static Graph gr[8];
static uint n_ticks = 0; // tick counter
static uint total_r_hits[4] = { 0, 0, 0, 0 }, total_w_hits[4] = { 0, 0, 0, 0 }; // cache hit counters
#define _oOo_oOo_ (O>=V|N>=800?0:(((N<<10)+O)*4)
uint* image[4], I,N,F,O,M,_O,V=2019; double K[999], Q[999];
float R(){I^=I<<13;I^=I>>17;I^=I<<5;return I*2.3283064365387e-10f*6-3;} // rng
int ant_x = 512, ant_y = 400, ant_dir = 0;

// -----------------------------------------------------------
// Visualization of the data stored in the memory hierarchy
// -----------------------------------------------------------
void Game::VisualizeMem()
{
	// draw the contents of the simulated DRAM; every pixel is 4 bytes
	// we bypass the Read/Write functions so we don't pollute the cache.
	for (int y = 0; y < 700; y++) for (int x = 0; x < 1024; x++)
	{
		int value = *((uint*)&((Memory*)mem.dram)->backdoor()[(x + y * 1024) * 4]);
		screen->Plot(x + 10, y + 10, (value >> 1) & 0x404040);
	}

	// draw the contents of the third cache level in red
	for (int i = 0; i < L3_SIZE / CACHELINEWIDTH; i++)
	{
		CacheLine& line = ((AssociativeCache*)mem.l3)->backdoor(i);
		int lineAddress = line.tag * CACHELINEWIDTH;
		int x = (lineAddress / 4) & 1023, y = (lineAddress / 4) / 1024;
		for (int j = 0; j < CACHELINEWIDTH / 4; j++)
			screen->Plot(x + 10 + j, y + 10, (((uint*)line.bytes)[j] & 0xff0000) | 0x200000);
	}

	// draw the contents of the second cache level in yellow
	for (int i = 0; i < L2_SIZE / CACHELINEWIDTH; i++)
	{
		CacheLine& line = ((AssociativeCache*)mem.l2)->backdoor(i);
		int lineAddress = line.tag * CACHELINEWIDTH;
		int x = (lineAddress / 4) & 1023, y = (lineAddress / 4) / 1024;
		for (int j = 0; j < CACHELINEWIDTH / 4; j++)
			screen->Plot(x + 10 + j, y + 10, (((uint*)line.bytes)[j] & 0xffff00) | 0x202000);
	}

	// draw the contents of the first cache level in green
	for (int i = 0; i < L1_SIZE / CACHELINEWIDTH; i++)
	{
		CacheLine& line = ((AssociativeCache*)mem.l1)->backdoor( i );
		int lineAddress = line.tag * CACHELINEWIDTH;
		int x = (lineAddress / 4) & 1023, y = (lineAddress / 4) / 1024;
		for (int j = 0; j < CACHELINEWIDTH / 4; j++)
			screen->Plot( x + 10 + j, y + 10, (((uint*)line.bytes)[j] & 0x00ff00) | 0x002000);
	}

	// draw hit/miss graphs
	screen->Print( "level 1 R/W", 1050, 10, 0xffffff );
	gr[0].Update( screen, 1050, 20, mem.l1->r_hit, mem.l1->r_miss );
	gr[1].Update( screen, 1170, 20, mem.l1->w_hit, mem.l1->w_miss );
	screen->Print("level 2 R/W", 1050, 90, 0xffffff);
	gr[2].Update( screen, 1050, 100, mem.l2->r_hit, mem.l2->r_miss );
	gr[3].Update( screen, 1170, 100, mem.l2->w_hit, mem.l2->w_miss );
	screen->Print("level 3 R/W", 1050, 170, 0xffffff);
	gr[4].Update( screen, 1050, 180, mem.l3->r_hit, mem.l3->r_miss );
	gr[5].Update( screen, 1170, 180, mem.l3->w_hit, mem.l3->w_miss );
	screen->Print("DRAM R/W", 1050, 250, 0xffffff);
	gr[6].Update( screen, 1050, 260, mem.dram->r_hit, mem.dram->r_miss );
	gr[7].Update( screen, 1170, 260, mem.dram->w_hit, mem.dram->w_miss );
}

// -----------------------------------------------------------
// Application initialization
// -----------------------------------------------------------
void Game::Init()
{
	for (V = 1024, F = 1, I = 1; I < 4; I++ );
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float )
{
	// clear the graphics window
	screen->Clear( 0x282828 );

	// reset cache hit/miss counters
	mem.ResetCounters();

	// update memory contents

#if ALGORITHM == SPIRAL
	// simple spiral							ACCESS PATTERN: STRUCTURED
	for (int i = 0; i < 10; i++)
	{
		int x = (int)(sinf( a ) * r + 512), y = (int)(cosf( a ) * r + 350);
		a += 0.01f; r -= 0.005f;
		if (r < -300) r = -300;
		mem.WriteUint( (x + y * 1024) * 4, 0xffff77 );
	}
#elif ALGORITHM == FRACTAL
	// the buddhabrot based on Paul Bourke		ACCESS PATTERN: MOSTLY RANDOM
	for(int G,M,T,E=0;++E<2;)for(G=0;++G<V
	<<7;){double B=0,y=0,t=R(),e,z=R();for
	(T=0;T<E<<8;){e=2*B*y+z,B=K[T]=B*B-y*y
	+t,y=Q[T++]=e;if(B*B+y*y>9){for(M=0;M<		// data access; to be cached:
	T;){O=400+.3*V*Q[M],N=.3*V*K[M++]+520;		mem.WriteUint _oOo_oOo_,
												mem.ReadUint _oOo_oOo_ )+545)
	/* END OF BLACK BOX CODE */;}break;}}}
#elif ALGORITHM == ANT
	// Langton's ant (https://en.wikipedia.org/wiki/Langton%27s_ant)
	for (int i = 0; i < 100; i++)
	{
		// Read color at current pixel
		uint state = mem.ReadUint((ant_x + ant_y * 1024) * 4);
		// Turn left or right
		if (state > 0) ant_dir--;
		else ant_dir++;
		ant_dir = (ant_dir + 4) % 4;
		// Flip color
		mem.WriteUint((ant_x + ant_y * 1024) * 4, 0x00ffffff ^ state);
		// Move
		switch (ant_dir) {
		case 0:
			ant_x++; break;
		case 1:
			ant_y++; break;
		case 2:
			ant_x--; break;
		case 3:
			ant_y--; break;
		}
		ant_x = (ant_x + 1024) % 1024;
		ant_y = (ant_y + 800) % 800;
	}
#endif

	// visualize the memory hierarchy
	VisualizeMem();

	// benchmark printout
	total_r_hits[0] += mem.l1->r_hit;
	total_r_hits[1] += mem.l2->r_hit;
	total_r_hits[2] += mem.l3->r_hit;
	total_r_hits[3] += mem.dram->r_hit;
	total_w_hits[0] += mem.l1->w_hit;
	total_w_hits[1] += mem.l2->w_hit;
	total_w_hits[2] += mem.l3->w_hit;
	total_w_hits[3] += mem.dram->w_hit;
	n_ticks++;
	if (n_ticks >= BENCHMARK_STEPS) {
		std::cout << "READ "
			<< total_r_hits[0] << ", "
			<< total_r_hits[1] << ", "
			<< total_r_hits[2] << ", "
			<< total_r_hits[3] << std::endl;
		std::cout << "WRITE "
			<< total_w_hits[0] << ", "
			<< total_w_hits[1] << ", "
			<< total_w_hits[2] << ", "
			<< total_w_hits[3] << std::endl << std::endl;
		n_ticks = 0;
	}
}