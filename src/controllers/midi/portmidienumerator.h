#pragma once

#include <QList>
#include <QString>
#include <memory>
#include <vector>

#include "controllers/controller.h"
#include "controllers/midi/midienumerator.h"

/// This class handles discovery and enumeration of DJ controllers that appear under the PortMIDI cross-platform API.
class PortMidiEnumerator : public MidiEnumerator {
    Q_OBJECT
  public:
    PortMidiEnumerator();
    ~PortMidiEnumerator() override;

    QList<Controller*> queryDevices() override;

  private:
    std::vector<std::unique_ptr<Controller>> m_devices;
};

// For testing.
bool shouldLinkInputToOutput(const QString& input_name,
        const QString& output_name);
