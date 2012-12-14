#include "alsawork.h"

void onError (int errorIndex)
{
	if (errorIndex < 0) {
		fprintf(stderr, "%s\n", snd_strerror(errorIndex));
		exit(1);
	}
}

void setAlsaVolume(const char *mixer, long volume)
{
	long min, max;
	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	int err = snd_mixer_open(&handle, 0);
	if (err < 0) {
		fprintf(stderr, "%s\n", snd_strerror(err));
		exit(1);
	}
	snd_ctl_t *ctl;
	snd_hctl_t * hctl;
	snd_ctl_card_info_t *cinfo;
	snd_ctl_card_info_alloca(&cinfo);
	int cardindex = -1;
	err = snd_card_next(&cardindex);
	onError(err);
	char str[64];
	sprintf(str, "hw:%i", cardindex);
	err= snd_ctl_open(&ctl, str, 0);
	onError(err);
	err = snd_ctl_card_info(ctl, cinfo);
	onError(err);
	err = snd_hctl_open_ctl(&hctl, ctl);
	onError(err);
	err = snd_mixer_attach_hctl(handle, hctl);
	onError(err);
	err = snd_mixer_selem_register(handle, NULL, NULL);
	onError(err);
	err = snd_mixer_load(handle);
	onError(err);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, mixer);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

	err = snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	onError(err);
	err = snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);
	onError(err);
	err = snd_mixer_close(handle);
	onError(err);
}
