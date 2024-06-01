#pragma once

#include <array>
// required for Qt-Macros
#include <qobjectdefs.h>

class WaveformWidgetType {
    Q_GADGET
  public:
    enum Type {
        // The order must not be changed because the waveforms are referenced
        // from the sorted preferences by a number.
        Empty = 0,
        Simple = 5,    // 5  Simple GL
        Filtered = 7,  // 7  Filtered GLSL
        HSV = 8,       // 8  HSV
        VSyncTest = 9, // 9  VSync GL
        RGB = 12,      // 12 RGB GLSL
        Stacked = 16,  // 16 RGB Stacked
    };
    Q_ENUM(Type)
    static constexpr std::array kValues = {
            WaveformWidgetType::Empty,
            WaveformWidgetType::Simple,
            WaveformWidgetType::Filtered,
            WaveformWidgetType::HSV,
            WaveformWidgetType::VSyncTest,
            WaveformWidgetType::RGB,
            WaveformWidgetType::Stacked,
    };
};

enum class WaveformWidgetBackend {
    None = 0,
    GL,
    GLSL,
#ifdef MIXXX_USE_QOPENGL
    AllShader,
#endif
};
