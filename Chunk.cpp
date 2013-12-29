#include "Chunk.h"
#include "Pool.h"

template <class TMeat>
void Chunk<TMeat>::returnToPool() {
	if (this->pool != nullptr) {
		this->pool->ReleaseChunk (this);
	} else {
		// No pool to go back to. Kill self:
		delete this;
	}
}
