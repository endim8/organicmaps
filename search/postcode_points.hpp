#pragma once

#include "search/mwm_context.hpp"
#include "search/search_index_values.hpp"
#include "search/token_slice.hpp"

#include "indexer/centers_table.hpp"
#include "indexer/mwm_set.hpp"
#include "indexer/trie.hpp"

#include "coding/reader.hpp"
#include "coding/write_to_sink.hpp"

#include "geometry/point2d.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace search
{
class PostcodePoints
{
public:
  enum class Version : uint8_t
  {
    V0 = 0,
    Latest = V0
  };

  struct Header
  {
    template <typename Sink>
    void Serialize(Sink & sink) const
    {
      CHECK_EQUAL(static_cast<uint8_t>(m_version), static_cast<uint8_t>(Version::V0), ());
      WriteToSink(sink, static_cast<uint8_t>(m_version));
      WriteToSink(sink, m_trieOffset);
      WriteToSink(sink, m_trieSize);
      WriteToSink(sink, m_pointsOffset);
      WriteToSink(sink, m_pointsSize);
    }

    void Read(Reader & reader);

    Version m_version = Version::V0;
    uint32_t m_trieOffset;
    uint32_t m_trieSize;
    uint32_t m_pointsOffset;
    uint32_t m_pointsSize;
  };

  PostcodePoints(MwmValue const & value);

  void Get(TokenSlice const & tokens, std::vector<m2::PointD> & points) const;

private:
  Header m_header;
  std::unique_ptr<CentersTable> m_points;
  std::unique_ptr<trie::Iterator<ValueList<FeatureIndexValue>>> m_root;
  std::unique_ptr<Reader> m_trieSubReader;
  std::unique_ptr<Reader> m_pointsSubReader;
};
}  // namespace search
