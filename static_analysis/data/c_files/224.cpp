void RTCSessionDescriptionRequestImpl::requestFailed(const String& error)
{
if (m_errorCallback)
        m_errorCallback->handleEvent(error);

clear();
}
