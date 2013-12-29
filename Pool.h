#ifndef Pool_h__
#define Pool_h__

#include <deque>
#include "Chunk.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#define POOL_SIZE_INCREMENT 3000
#define POOL_SIZE_LOW_THRESHOLD 100


// Forward Declarations:
template <class TMeat>
class MChunk;

template <class TMeat>
class Pool
{
public:
	Pool ();
	~Pool ();

	MChunk<TMeat> GetManagedChunk ();
	void ReleaseChunk (Chunk<TMeat> *_chunk);
	int GetFreeCount () { return this->chunks_free.size (); }
	int GetInuseCount () { return this->chunks_inuse; }

private:
	std::deque<Chunk<TMeat> *> chunks_free;
	boost::mutex chunks_free_mutex;
	int chunks_inuse;

	// Utility Functions:
	void makeMoreChunks ();
};



////////////////////////////////////////////////////////////////////////////////

template <class TMeat>
Pool<TMeat>::Pool() {
	// std::cout << "\nInitializing Pool ...";

	this->chunks_inuse = 0;

	boost::mutex::scoped_lock lock_on_chunks_free_queue (this->chunks_free_mutex);
	// Create an initial pool:
	this->makeMoreChunks ();
	// Scoped lock on this->chunks_free_mutex falls off
}

template <class TMeat>
Pool<TMeat>::~Pool() {
	// std::cout << "\nDestroying Pool ...";

	// Free the pools:
	for (auto chunk_it = this->chunks_free.begin (); chunk_it != this->chunks_free.end (); ++chunk_it) {
		Chunk<TMeat> *chunk = (*chunk_it);
		delete chunk;
	}
	this->chunks_free.clear ();
}

template <class TMeat>
MChunk<TMeat> Pool<TMeat>::GetManagedChunk() {
	boost::mutex::scoped_lock lock_on_chunks_free_queue (this->chunks_free_mutex);

	if (this->chunks_free.size () == 0) {
		// Out of free chunks; must make more
		this->makeMoreChunks ();
	}

	// ASSERT (this->chunks_free.size () != 0, "No more chunks");
	Chunk<TMeat> *chunk = this->chunks_free.front ();
	this->chunks_free.pop_front ();

	if (this->chunks_free.size () < POOL_SIZE_LOW_THRESHOLD) {
		// Low on free chunks; must make more
		this->makeMoreChunks ();
	}

	this->chunks_inuse++;
	MChunk<TMeat> mchunk (chunk);
	return (mchunk);
	// Scoped lock on this->chunks_free_mutex falls off
}

template <class TMeat>
void Pool<TMeat>::ReleaseChunk( Chunk<TMeat> *_chunk ) {
	// std::cout << "\nIn pool: Moving chunk to free list";
	boost::mutex::scoped_lock lock_on_chunks_free_queue (this->chunks_free_mutex);

	this->chunks_free.push_back (_chunk);
	this->chunks_inuse--;
	// ASSERT (this->chunks_inuse >= 0, "ASSERT (chunks_inuse >= 0)");
	// Scoped lock on this->chunks_free_mutex falls off
}

template <class TMeat>
void Pool<TMeat>::makeMoreChunks() {
	std::cout << "\nLow on chunks, making " << POOL_SIZE_INCREMENT << " more";

	// Create a pool of size POOL_SIZE_DEFAULT and add them all to the free list:
	for (int i = 0; i < POOL_SIZE_INCREMENT; i++) {
		TMeat *meat = new TMeat ();
		Chunk<TMeat> *chunk = new Chunk<TMeat> ();
		chunk->SetMeat (meat);
		chunk->SetPool (this);
		this->chunks_free.push_back (chunk);
	}
}

#endif // Pool_h__
