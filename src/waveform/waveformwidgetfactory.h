#pragma once

#include <QObject>
#include <QSurfaceFormat>
#include <QVector>
#include <memory>
#include <vector>

#include "preferences/usersettings.h"
#include "skin/legacy/skincontext.h"
#include "util/parented_ptr.h"
#include "util/performancetimer.h"
#include "util/singleton.h"
#include "waveform/renderers/allshader/waveformrenderersignalbase.h"
#include "waveform/vsyncthread.h"
#include "waveform/widgets/waveformwidgetabstract.h"
#include "waveform/widgets/waveformwidgettype.h"
#include "waveform/widgets/waveformwidgetvars.h"

class WVuMeterLegacy;
class WVuMeterBase;
class WWaveformViewer;
class GuiTick;
class VisualsManager;

class WaveformWidgetAbstractHandle {
  public:
    WaveformWidgetAbstractHandle(WaveformWidgetType::Type type,
            QList<WaveformWidgetBackend> backends
#ifdef MIXXX_USE_QOPENGL
            ,
            int supportedOptions
#endif
            )
            : m_type(type), m_backends(std::move(backends))
#ifdef MIXXX_USE_QOPENGL
              ,
              m_supportedOption(supportedOptions)
#endif
    {
    }

    WaveformWidgetType::Type getType() const { return m_type;}
    const QList<WaveformWidgetBackend>& getBackend() const {
        return m_backends;
    }
    bool supportAcceleration() const {
        for (auto backend : m_backends) {
            if (backend == WaveformWidgetBackend::GL ||
                    backend == WaveformWidgetBackend::GLSL
#ifdef MIXXX_USE_QOPENGL
                    || backend == WaveformWidgetBackend::AllShader
#endif
            ) {
                return true;
            }
        }
        return false;
    }
    bool supportSoftware() const {
        return m_backends.contains(WaveformWidgetBackend::None);
    }

#ifdef MIXXX_USE_QOPENGL
    allshader::WaveformRendererSignalBase::Options supportedOptions(
            WaveformWidgetBackend backend) const {
        return backend == WaveformWidgetBackend::AllShader
                ? m_supportedOption
                : allshader::WaveformRendererSignalBase::Option::None;
    }
#endif

    QString getDisplayName() const;

  private:
    WaveformWidgetType::Type m_type;
    QList<WaveformWidgetBackend> m_backends;
#ifdef MIXXX_USE_QOPENGL
    // Only relevant for Allshader (accelerated) backend. Other backends don't implement options
    allshader::WaveformRendererSignalBase::Options m_supportedOption;
#endif

    friend class WaveformWidgetFactory;
};

//########################################

class WaveformWidgetFactory : public QObject, public Singleton<WaveformWidgetFactory> {
    Q_OBJECT

    // TODO: make it impossible to violoate m_waveformWidget.viewer == m_waveformViewer invariant
    struct WaveformWidgetHolder {
        parented_ptr<WaveformWidgetAbstract> m_waveformWidget = nullptr;
        WWaveformViewer* m_waveformViewer = nullptr;
        QDomNode m_skinNodeCache{};
        SkinContext m_skinContextCache{UserSettingsPointer(), QString()};
    };

  public:
    //TODO merge this enum with the waveform analyzer one
    enum FilterIndex { All = 0, Low = 1, Mid = 2, High = 3, FilterCount = 4};

    bool setConfig(UserSettingsPointer config);

    /// Creates the waveform widget using the type set with setWidgetType
    /// and binds it to the viewer.
    /// Deletes older widget and resets positions to config defaults.
    bool setWaveformWidget(
            WWaveformViewer* viewer,
            const QDomElement &node,
            const SkinContext& parentContext);

    void setFrameRate(int frameRate);
    int getFrameRate() const { return m_frameRate;}
//    bool getVSync() const { return m_vSyncType;}
    void setEndOfTrackWarningTime(int endTime);
    int getEndOfTrackWarningTime() const { return m_endOfTrackWarningTime;}

    bool isOpenGlAvailable() const { return m_openGlAvailable;}
    bool isOpenGlesAvailable() const { return m_openGlesAvailable;}
    QString getOpenGLVersion() const { return m_openGLVersion;}

    bool isOpenGlShaderAvailable() const { return m_openGLShaderAvailable;}

    WaveformWidgetBackend preferredBackend() const;
    static WaveformWidgetType::Type defaultType() {
        return WaveformWidgetType::RGB;
    }

    /// Sets the widget type and saves it to configuration.
    /// Returns false and sets EmtpyWaveform if type is invalid
    bool setWidgetType(WaveformWidgetType::Type type);
    /// Changes the widget type to that loaded from config and recreates them.
    /// Used as a workaround on Windows due to a problem with GL and QT 5.14.2
    bool setWidgetTypeFromConfig();
    /// Changes the widget type and recreates them. Used from the preferences
    /// dialog.
    bool setWidgetTypeFromHandle(int handleIndex, bool force = false);
    WaveformWidgetType::Type getType() const { return m_type;}
    int getHandleIndex() {
        return findHandleIndexFromType(m_type);
    }
    int findHandleIndexFromType(WaveformWidgetType::Type type);
    bool widgetTypeSupportsUntilMark() const;
    void setUntilMarkShowBeats(bool value);
    void setUntilMarkShowTime(bool value);
    void setUntilMarkAlign(Qt::Alignment align);
    void setUntilMarkTextPointSize(int value);

    bool getUntilMarkShowBeats() const {
        return m_untilMarkShowBeats;
    }
    bool getUntilMarkShowTime() const {
        return m_untilMarkShowTime;
    }
    Qt::Alignment getUntilMarkAlign() const {
        return m_untilMarkAlign;
    }
    int getUntilMarkTextPointSize() const {
        return m_untilMarkTextPointSize;
    }

    static Qt::Alignment toUntilMarkAlign(int index);
    static int toUntilMarkAlignIndex(Qt::Alignment align);

    /// Returns the desired surface format for the OpenGLWindow
    static QSurfaceFormat getSurfaceFormat(UserSettingsPointer config = nullptr);

  protected:
    bool setWidgetType(
            WaveformWidgetType::Type type,
            WaveformWidgetType::Type* pCurrentType);

  public:
    void setDefaultZoom(double zoom);
    double getDefaultZoom() const { return m_defaultZoom;}

    void setZoomSync(bool sync);
    int isZoomSync() const { return m_zoomSync;}

    void setDisplayBeatGridAlpha(int alpha);
    int getBeatGridAlpha() const { return m_beatGridAlpha; }

    void setVisualGain(FilterIndex index, double gain);
    double getVisualGain(FilterIndex index) const;

    void setOverviewNormalized(bool normalize);
    int isOverviewNormalized() const { return m_overviewNormalized;}

    const QVector<WaveformWidgetAbstractHandle> getAvailableTypes() const { return m_waveformWidgetHandles;}
    void getAvailableVSyncTypes(QList<QPair<int, QString>>* list);
    void destroyWidgets();

    void addVuMeter(WVuMeterLegacy* pWidget);
    void addVuMeter(WVuMeterBase* pWidget);

    void startVSync(GuiTick* pGuiTick, VisualsManager* pVisualsManager);

    void setPlayMarkerPosition(double position);
    double getPlayMarkerPosition() const { return m_playMarkerPosition; }

    void notifyZoomChange(WWaveformViewer *viewer);
  signals:
    void waveformUpdateTick();
    void waveformMeasured(float frameRate, int droppedFrames);
    void renderSpinnies(VSyncThread*);
    void swapSpinnies();
    void renderVuMeters(VSyncThread*);
    void swapVuMeters();

  public slots:
    void slotSkinLoaded();

  protected:
    WaveformWidgetFactory();

    friend class Singleton<WaveformWidgetFactory>;

  private slots:
    void render();
    void swap();
    void swapAndRender();
    void slotFrameSwapped();

  private:
    void renderSelf();
    void swapSelf();

    void addHandle(
            QHash<WaveformWidgetType::Type, QList<WaveformWidgetBackend>>&
                    collectedHandles,
            WaveformWidgetType::Type type,
            const WaveformWidgetVars& vars) const;
    void evaluateWidgets();
    template<typename WaveformT>
    QString buildWidgetDisplayName() const;
    parented_ptr<WaveformWidgetAbstract> createAllshaderWaveformWidget(
            WaveformWidgetType::Type type, WWaveformViewer* viewer);
    parented_ptr<WaveformWidgetAbstract> createWaveformWidget(
            WaveformWidgetType::Type type, WWaveformViewer* viewer);
    int findIndexOf(WWaveformViewer* viewer) const;

    //All type of available widgets

    QVector<WaveformWidgetAbstractHandle> m_waveformWidgetHandles;

    //Currently in use widgets/visual/node
    std::vector<WaveformWidgetHolder> m_waveformWidgetHolders;

    WaveformWidgetType::Type m_type;
    WaveformWidgetType::Type m_configType;

    UserSettingsPointer m_config;

    bool m_skipRender;
    int m_frameRate;
    int m_endOfTrackWarningTime;
    double m_defaultZoom;
    bool m_zoomSync;
    double m_visualGain[FilterCount];
    bool m_overviewNormalized;

    bool m_untilMarkShowBeats;
    bool m_untilMarkShowTime;
    Qt::Alignment m_untilMarkAlign;
    int m_untilMarkTextPointSize;

    bool m_openGlAvailable;
    bool m_openGlesAvailable;
    QString m_openGLVersion;
    bool m_openGLShaderAvailable;
    int m_beatGridAlpha;

    std::unique_ptr<VSyncThread> m_pVSyncThread;
    GuiTick* m_pGuiTick;  // not owned
    VisualsManager* m_pVisualsManager;  // not owned

    parented_ptr<WaveformWidgetAbstract> createFilteredWaveformWidget(WWaveformViewer* viewer);
    parented_ptr<WaveformWidgetAbstract> createHSVWaveformWidget(WWaveformViewer* viewer);
    parented_ptr<WaveformWidgetAbstract> createRGBWaveformWidget(WWaveformViewer* viewer);
    parented_ptr<WaveformWidgetAbstract> createStackedWaveformWidget(WWaveformViewer* viewer);
    parented_ptr<WaveformWidgetAbstract> createSimpleWaveformWidget(WWaveformViewer* viewer);
    parented_ptr<WaveformWidgetAbstract> createVSyncTestWaveformWidget(WWaveformViewer* viewer);

    //Debug
    PerformanceTimer m_time;
    float m_frameCnt;
    double m_actualFrameRate;
    int m_vSyncType;
    double m_playMarkerPosition;
};
