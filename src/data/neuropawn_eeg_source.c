# include "data/neuropawn_eeg_source.h"
# include "storage.h"
# include "microsecond_timer.h"
# include "../../thirdparty/tiny_bci/producer/neuropawn_producer.h"

static NeuroPawnProducer producer;
static NeuroPawnProducerConfig config;

void connectNeuropawnEEGSource(const char *port)
{
    if (SAMPLE_RATE != (int)NEUROPAWN_SRATE || CHANNEL_COUNT != NEUROPAWN_N_CHANNELS)
    {
        fprintf(stderr, "NeuroPawn requires 8 channels at 125 Hz in 'eeg_source.h'\n");
        exit(EXIT_FAILURE);
    }

    config.port         = port;
    config.srate        = NEUROPAWN_SRATE;
    config.n_channels   = NEUROPAWN_N_CHANNELS;
    config.gain         = NEUROPAWN_DEFAULT_GAIN;
    config.cmd_pause_ms = NEUROPAWN_CMD_PAUSE_MS;
    for (size_t channel = 0; channel < NEUROPAWN_N_CHANNELS; channel++)
        config.channel_enabled[channel] = true;

    np_init(&producer, &config);
    producer.trigger_gen = NULL;

    TBCI_Status status = producer.base.init((TBCI_Producer *)&producer, &tbciInputs, &tbciContext);
    if (status != TBCI_OK)
    {
        fprintf(stderr, "neuropawn: failed to connect on %s | code: %d\n", port, status);
        exit(EXIT_FAILURE);
    }
}

void resetNeuropawnEEGSource()
{
    np_reset(&producer);
    producer.state.timestamp_us = getCurrentMicrosecondTimestamp();
}

void updateNeuropawnEEGSource()
{
    TBCI_Status status = producer.base.tick((TBCI_Producer *)&producer, &tbciInputs, &tbciContext);
    if (status != TBCI_OK && status != TBCI_ERR_EMPTY)
    {
        fprintf(stderr, "neuropawn: tick error | code: %d\n", status);
    }
}

void disconnectNeuropawnEEGSource()
{
    np_close(&producer);
}