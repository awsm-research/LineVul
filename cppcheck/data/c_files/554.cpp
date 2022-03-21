void makeCurrent()
{
#if PLATFORM(QT)
        m_detachedContext = QOpenGLContext::currentContext();
        if (m_detachedContext)
            m_detachedSurface = m_detachedContext->surface();
        if (m_surface && m_glContext)
            m_glContext->makeCurrent(m_surface.get());
#elif PLATFORM(EFL)
m_detachedContext = glXGetCurrentContext();
m_detachedSurface = glXGetCurrentDrawable();
if (m_surface && m_glContext)
glXMakeCurrent(m_display, m_surface, m_glContext);
#endif
}
