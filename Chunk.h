#ifndef Chunk_h__
#define Chunk_h__

// Forward Declarations:
template <class TMeat>
class Pool;

template <class TMeat>
class Chunk {
public:
	TMeat *meat;		// Pointer to the actual little bit of data whose pool is being managed

	Chunk ();

	inline void SetPool (Pool<TMeat> *_pool)	{ this->pool = _pool; }
	inline void SetMeat (TMeat *_meat)		{ this->meat = _meat; }
	inline void Use () { this->use_count++; }
	inline void DisUse () {
		this->use_count--;
		if (this->use_count <= 0) {
			this->returnToPool ();
		}
	}

	inline int GetUseCount ()		{ return this->use_count; }

private:
	int use_count;
	Pool<TMeat> *pool;

	// Utility Functions:
	void returnToPool ();
};



////////////////////////////////////////////////////////////////////////////////

template <class TMeat>
Chunk<TMeat>::Chunk() {
	this->meat = nullptr;
	this->pool = nullptr;
	this->use_count = 0;
}

#include "Chunk.cpp"

#endif // Chunk_h__
