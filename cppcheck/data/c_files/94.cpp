void bdt_enable(void)
{
bdt_log("ENABLE BT");
if (bt_enabled) {

bdt_log("Bluetooth is already enabled");
return;
}
    status = sBtInterface->enable();

check_return_status(status);
}
