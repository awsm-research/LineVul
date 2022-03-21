void ath_tx_aggr_sleep(struct ieee80211_sta *sta, struct ath_softc *sc,
struct ath_node *an)
{
struct ath_atx_tid *tid;
struct ath_atx_ac *ac;
struct ath_txq *txq;
bool buffered;
int tidno;

for (tidno = 0, tid = &an->tid[tidno];
tidno < IEEE80211_NUM_TIDS; tidno++, tid++) {

		if (!tid->sched)
			continue;
ac = tid->ac;
txq = ac->txq;

ath_txq_lock(sc, txq);

buffered = ath_tid_has_buffered(tid);

tid->sched = false;
list_del(&tid->list);

if (ac->sched) {
ac->sched = false;
list_del(&ac->list);
}

ath_txq_unlock(sc, txq);

ieee80211_sta_set_buffered(sta, tidno, buffered);
}
}
