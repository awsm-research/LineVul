status_t MediaPlayer::setDataSource(
const sp<IMediaHTTPService> &httpService,
const char *url, const KeyedVector<String8, String8> *headers)
{

ALOGV("setDataSource(%s)", url);
status_t err = BAD_VALUE;
if (url != NULL) {
        const sp<IMediaPlayerService>& service(getMediaPlayerService());
if (service != 0) {
sp<IMediaPlayer> player(service->create(this, mAudioSessionId));
if ((NO_ERROR != doSetRetransmitEndpoint(player)) ||
(NO_ERROR != player->setDataSource(httpService, url, headers))) {
player.clear();
}
err = attachNewPlayer(player);
}
}
return err;
}
