#include "effects/chains/pergroupeffectchain.h"

#include "effects/effectsmanager.h"

PerGroupEffectChain::PerGroupEffectChain(const QString& group,
        const QString& chainSlotGroup,
        SignalProcessingStage stage,
        EffectsManager* pEffectsManager,
        EffectsMessengerPointer pEffectsMessenger)
        : EffectChain(chainSlotGroup,
                  pEffectsManager,
                  pEffectsMessenger,
                  stage) {
    // Set the chain to be fully wet.
    m_pControlChainMix->set(1.0);
    sendParameterUpdate();

    // TODO(rryan): remove.
    GroupHandle handleAndGroup;
    DEBUG_ASSERT(!handleAndGroup);
    for (GroupHandle handle_group :
            m_pEffectsManager->registeredInputChannels()) {
        if (handle_group.name() == group) {
            handleAndGroup = handle_group;
            break;
        }
    }
    DEBUG_ASSERT(handleAndGroup);

    // Register this channel alone with the chain slot.
    registerInputChannel(handleAndGroup);
    enableForInputChannel(handleAndGroup);
}
