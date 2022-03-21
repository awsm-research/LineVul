Chapters::Chapters(
    Segment* pSegment,
    long long payload_start,
    long long payload_size,
    long long element_start,
    long long element_size) :
    m_pSegment(pSegment),
    m_start(payload_start),
    m_size(payload_size),
    m_element_start(element_start),
    m_element_size(element_size),
    m_editions(NULL),
    m_editions_size(0),
    m_editions_count(0)
{
}
