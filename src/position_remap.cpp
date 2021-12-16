#include "position_remap.h"

#include "vec3.h"
#include "hash_table.h"
#include "hash.h"

struct PositionHasher {
	const Vec3* positions;
	static constexpr uint32_t empty_key = ~static_cast<uint32_t>(0);
	size_t   hash(uint32_t key) const;
	bool is_empty(uint32_t key) const {return (key == empty_key);}
	bool is_equal(uint32_t key1, uint32_t key2) const;
};
typedef HashTable<uint32_t, uint32_t, PositionHasher> PositionRemap;

inline size_t PositionHasher::hash(uint32_t key) const
{
	static_assert(sizeof(uint32_t) == sizeof(float));

	return position_hash((const float *)(positions + key));
}

inline bool PositionHasher::is_equal(uint32_t key1, uint32_t key2) const
{
	return (positions[key1] == positions[key2]);
}


void
build_position_remap(const Vec3* positions, uint32_t num_pos, uint32_t *remap)
{
	PositionHasher hasher = {positions};
	PositionRemap position_remap(num_pos, hasher);

	for (size_t i = 0; i < num_pos; ++i)
	{
		uint32_t *p = position_remap.get_or_set(i, i);
		remap[i] = p ? *p : i;
	}
}

