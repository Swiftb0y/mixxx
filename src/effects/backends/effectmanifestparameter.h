#pragma once

#include <QSharedPointer>
#include <QString>
#include <QVariant>
#include <QtDebug>

#include "effects/defs.h"
#include "util/assert.h"
#include "util/compatibility/qhash.h"

class EffectManifestParameter;
typedef QSharedPointer<EffectManifestParameter> EffectManifestParameterPointer;

class EffectManifestParameter {
  public:
    enum class ParameterType : int {
        Knob,
        Button,
        NumTypes,
    };

    enum class ValueScaler : int {
        Unknown = 0,
        Linear,
        LinearInverse,
        Logarithmic,
        LogarithmicInverse,
        /// A step rotary, steps given by m_steps are arranged with equal
        /// distance on scale
        Integral,
        /// For button and enum controls, not accessible from many controllers,
        /// no linking to meta knob
        Toggle,
    };

    enum class UnitsHint : int {
        Unknown = 0,
        /// Multiples of a Beat
        Beats,
        Beat,
        Bar,
        BPM,
        Cent,
        Centimetre,
        Coefficient,
        Decibel,
        Degree,
        Frame,
        Hertz,
        Inch,
        KiloHertz,
        Kilometer,
        Meter,
        MegaHertz,
        Midinote,
        Mile,
        Minute,
        Millmeter,
        Millisecond,
        Octave,
        Percentage,
        /// Fraction of the Sample Rate
        SampleRate,
        Seconds,
        Semitone12tet,
        Time, // units?
    };

    static UnitsHint lv2UnitToUnitsHint(const QString& lv2Unit) {
        if (lv2Unit == QLatin1String("bar")) {
            return UnitsHint::Bar;
        } else if (lv2Unit == QLatin1String("beat")) {
            return UnitsHint::Beat;
        } else if (lv2Unit == QLatin1String("bpm")) {
            return UnitsHint::BPM;
        } else if (lv2Unit == QLatin1String("cent")) {
            return UnitsHint::Cent;
        } else if (lv2Unit == QLatin1String("cm")) {
            return UnitsHint::Centimetre;
        } else if (lv2Unit == QLatin1String("coef")) {
            return UnitsHint::Coefficient;
        } else if (lv2Unit == QLatin1String("db")) {
            return UnitsHint::Decibel;
        } else if (lv2Unit == QLatin1String("degree")) {
            return UnitsHint::Degree;
        } else if (lv2Unit == QLatin1String("frame")) {
            return UnitsHint::Frame;
        } else if (lv2Unit == QLatin1String("hz")) {
            return UnitsHint::Hertz;
        } else if (lv2Unit == QLatin1String("inch")) {
            return UnitsHint::Inch;
        } else if (lv2Unit == QLatin1String("khz")) {
            return UnitsHint::KiloHertz;
        } else if (lv2Unit == QLatin1String("km")) {
            return UnitsHint::Kilometer;
        } else if (lv2Unit == QLatin1String("m")) {
            return UnitsHint::Meter;
        } else if (lv2Unit == QLatin1String("mhz")) {
            return UnitsHint::MegaHertz;
        } else if (lv2Unit == QLatin1String("midiNote")) {
            return UnitsHint::Midinote;
        } else if (lv2Unit == QLatin1String("mile")) {
            return UnitsHint::Mile;
        } else if (lv2Unit == QLatin1String("min")) {
            return UnitsHint::Minute;
        } else if (lv2Unit == QLatin1String("mm")) {
            return UnitsHint::Millmeter;
        } else if (lv2Unit == QLatin1String("ms")) {
            return UnitsHint::Millisecond;
        } else if (lv2Unit == QLatin1String("oct")) {
            return UnitsHint::Octave;
        } else if (lv2Unit == QLatin1String("pc")) {
            return UnitsHint::Percentage;
        } else if (lv2Unit == QLatin1String("s")) {
            return UnitsHint::Seconds;
        } else if (lv2Unit == QLatin1String("semitone12TET")) {
            return UnitsHint::Semitone12tet;
        } else {
            return UnitsHint::Unknown;
        }
    }

    static QString unitsHintToString(const UnitsHint unitsHint) {
        switch (unitsHint) {
        case UnitsHint::BPM:
            return QStringLiteral("BPM");
        case UnitsHint::Cent:
            return QStringLiteral("ct");
        case UnitsHint::Centimetre:
            return QStringLiteral("cm");
        case UnitsHint::Decibel:
            return QStringLiteral("dB");
        case UnitsHint::Degree:
            return QStringLiteral("°");
        case UnitsHint::Frame:
            return QStringLiteral("f");
        case UnitsHint::Hertz:
            return QStringLiteral("Hz");
        case UnitsHint::Inch:
            return QStringLiteral("in");
        case UnitsHint::KiloHertz:
            return QStringLiteral("kHz");
        case UnitsHint::Kilometer:
            return QStringLiteral("km");
        case UnitsHint::Meter:
            return QStringLiteral("m");
        case UnitsHint::MegaHertz:
            return QStringLiteral("MHz");
        case UnitsHint::Mile:
            return QStringLiteral("mi");
        case UnitsHint::Minute:
            return QStringLiteral("min");
        case UnitsHint::Millmeter:
            return QStringLiteral("mm");
        case UnitsHint::Millisecond:
            return QStringLiteral("ms");
        case UnitsHint::Octave:
            return QStringLiteral("oct");
        case UnitsHint::Percentage:
            return QStringLiteral("%");
        case UnitsHint::Seconds:
            return QStringLiteral("s");
        case UnitsHint::Semitone12tet:
            return QStringLiteral("semi");
        case UnitsHint::Unknown:
        case UnitsHint::Beat:
        case UnitsHint::Beats:
        case UnitsHint::Bar:
        case UnitsHint::Coefficient:
        case UnitsHint::Midinote:
        case UnitsHint::SampleRate:
        default:
            return QLatin1String("");
        }
    }

    enum class LinkType : int {
        /// Not controlled by the meta knob
        None = 0,
        /// Controlled by the meta knob as it is
        Linked,
        /// Controlled by the left side of the meta knob
        LinkedLeft,
        /// Controlled by the right side of the meta knob
        LinkedRight,
        /// Controlled by both sides of the meta knob
        LinkedLeftRight,
        NumLinkTypes,
    };

    static QString LinkTypeToString(LinkType type) {
        switch (type) {
        case LinkType::Linked:
            return QLatin1String("LINKED");
        case LinkType::LinkedLeft:
            return QLatin1String("LINKED_LEFT");
        case LinkType::LinkedRight:
            return QLatin1String("LINKED_RIGHT");
        case LinkType::LinkedLeftRight:
            return QLatin1String("LINKED_LEFT_RIGHT");
        default:
            return QLatin1String("NONE");
        }
    }

    static LinkType LinkTypeFromString(const QString& string) {
        if (string == QLatin1String("LINKED")) {
            return LinkType::Linked;
        } else if (string == QLatin1String("LINKED_LEFT")) {
            return LinkType::LinkedLeft;
        } else if (string == QLatin1String("LINKED_RIGHT")) {
            return LinkType::LinkedRight;
        } else if (string == QLatin1String("LINKED_LEFT_RIGHT")) {
            return LinkType::LinkedLeftRight;
        } else {
            return LinkType::None;
        }
    }

    enum class LinkInversion : int {
        NotInverted = 0,
        Inverted = 1
    };

    EffectManifestParameter()
            : m_valueScaler(ValueScaler::Unknown),
              m_unitsHint(UnitsHint::Unknown),
              m_defaultLinkType(LinkType::None),
              m_defaultLinkInversion(LinkInversion::NotInverted),
              m_neutralPointOnScale(0.0),
              m_default(0),
              m_minimum(0),
              m_maximum(1.0) {
    }

    ~EffectManifestParameter() {
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Parameter Information
    ////////////////////////////////////////////////////////////////////////////////

    const QString& id() const {
        return m_id;
    }
    void setId(const QString& id) {
        m_id = id;
    }

    const QString& name() const {
        return m_name;
    }
    void setName(const QString& name) {
        m_name = name;
    }

    const QString& shortName() const {
        return m_shortName;
    }
    void setShortName(const QString& shortName) {
        m_shortName = shortName;
    }

    const QString& description() const {
        return m_description;
    }
    void setDescription(const QString& description) {
        m_description = description;
    }

    int index() const {
        return m_iIndex;
    }
    void setIndex(int index) {
        m_iIndex = index;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Usage hints
    ////////////////////////////////////////////////////////////////////////////////

    const ParameterType& parameterType() const {
        return m_parameterType;
    }

    ValueScaler valueScaler() const {
        return m_valueScaler;
    }
    void setValueScaler(ValueScaler valueScaler) {
        m_valueScaler = valueScaler;
        if (valueScaler == ValueScaler::Toggle) {
            setParameterType(ParameterType::Button);
        } else {
            setParameterType(ParameterType::Knob);
        }
    }

    UnitsHint unitsHint() const {
        return m_unitsHint;
    }
    const QString unitString() const {
        return unitsHintToString(m_unitsHint);
    }
    void setUnitsHint(UnitsHint unitsHint) {
        m_unitsHint = unitsHint;
    }

    LinkType defaultLinkType() const {
        return m_defaultLinkType;
    }
    void setDefaultLinkType(const LinkType linkType) {
        m_defaultLinkType = linkType;
    }

    LinkInversion defaultLinkInversion() const {
        return m_defaultLinkInversion;
    }
    void setDefaultLinkInversion(const LinkInversion linkInversion) {
        m_defaultLinkInversion = linkInversion;
    }

    /// Neutral Point On Scale is the parameter in the range 0 .. 1 on the knob that
    /// is adopted as neutral when controlled by the meta knob.
    /// This is allows to link the meta knob in a way that two effects are
    /// cranked in simultaneous, or in case of a split filter like meta knob,
    /// both effects are neutral at meta knob center.
    /// A EQ Gain has usually a neutral point of 0.5 (0 dB) while a delay knob
    /// has a neutral point of 0.0 (no delay)
    /// A EQ Gain knob cannot be used on a split meta knob.
    double neutralPointOnScale() const {
        return m_neutralPointOnScale;
    }
    void setNeutralPointOnScale(double neutralPoint) {
        m_neutralPointOnScale = neutralPoint;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Value Settings
    ////////////////////////////////////////////////////////////////////////////////

    const double& getDefault() const {
        return m_default;
    }

    const double& getMinimum() const {
        return m_minimum;
    }

    const double& getMaximum() const {
        return m_maximum;
    }

    /// Minimum, default, and maximum are set together in one function so their
    /// validity only needs to be checked once.
    void setRange(const double& minimum, const double& defaultValue, const double& maximum) {
        VERIFY_OR_DEBUG_ASSERT(minimum <= defaultValue && defaultValue <= maximum) {
            qWarning() << "EffectManifestParameter" << m_name
                       << "tried to set invalid parameter range:"
                       << "minimum" << minimum
                       << "default" << defaultValue
                       << "maximum" << maximum;
            return;
        }
        m_minimum = minimum;
        m_default = defaultValue;
        m_maximum = maximum;
    }

    void appendStep(const QPair<QString, double>& step) {
        m_steps.append(step);
    }
    const QList<QPair<QString, double>>& getSteps() const {
        return m_steps;
    }

  private:
    void setParameterType(const ParameterType parameterType) {
        m_parameterType = parameterType;
    }

    QString debugString() const {
        return QString("EffectManifestParameter(%1)").arg(m_id);
    }

    QString m_id;
    QString m_name;
    QString m_shortName;
    QString m_description;
    int m_iIndex;

    ParameterType m_parameterType;
    ValueScaler m_valueScaler;
    UnitsHint m_unitsHint;
    LinkType m_defaultLinkType;
    LinkInversion m_defaultLinkInversion;
    double m_neutralPointOnScale;

    double m_default;
    double m_minimum;
    double m_maximum;

    /// Used to describe steps of
    /// CONTROL_INTEGRAL and CONTROL_TOGGLE
    /// effect parameters
    /// Each pair has the following form:
    /// name - value
    QList<QPair<QString, double>> m_steps;
};

QDebug operator<<(QDebug dbg, const EffectManifestParameter& parameter);

typedef EffectManifestParameter::ParameterType EffectParameterType;

inline qhash_seed_t qHash(const EffectParameterType& parameterType) {
    return static_cast<qhash_seed_t>(parameterType);
}
