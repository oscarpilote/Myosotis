#include "mesh.h"
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
build_position_remap(const Mesh& mesh, const MeshData& data, uint32_t *remap)
{
	PositionHasher hasher = {data.positions + mesh.vertex_offset};
	PositionRemap position_remap(mesh.vertex_count, hasher);

	for (size_t i = 0; i < mesh.vertex_count; ++i)
	{
		uint32_t *p = position_remap.get_or_set(i, i);
		remap[i] = p ? *p : i;
	}
}

