const Chapters::Display* Chapters::Atom::GetDisplay(int index) const
{
    if (index < 0)
        return NULL;
    if (index >= m_displays_count)
        return NULL;
    return m_displays + index;
}
