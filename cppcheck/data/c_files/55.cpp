void CCLayerTreeHostTest::endTest()
{
// If we are called from the CCThread, re-call endTest on the main thread.
if (!isMainThread())
        CCMainThread::postTask(createMainThreadTask(this, &CCLayerTreeHostTest::endTest));
else {
// For the case where we endTest during beginTest(), set a flag to indicate that
// the test should end the second beginTest regains control.
if (m_beginning)
m_endWhenBeginReturns = true;
else
onEndTest(static_cast<void*>(this));
}
}
