bool Block::IsInvisible() const
{
    return bool(int(m_flags & 0x08) != 0);
}
