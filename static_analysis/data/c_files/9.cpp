SchedulerObject::_continue(std::string key, std::string &/*reason*/, std::string &text)
{
PROC_ID id = getProcByString(key.c_str());
       if (id.cluster < 0 || id.proc < 0) {
dprintf(D_FULLDEBUG, "Remove: Failed to parse id: %s\n", key.c_str());
text = "Invalid Id";
return false;
}

scheduler.enqueueActOnJobMyself(id,JA_CONTINUE_JOBS,true);

return true;
}
