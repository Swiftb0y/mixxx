#pragma once

#include <QString>
#include <QtDebug>

#include "util/assert.h"
#include "util/compatibility/qhash.h"

namespace mixxx {
namespace grouphandle_private {
struct GroupHandleImpl;
} // namespace grouphandle_private
} // namespace mixxx

/// Finalize the registration of groups
///
/// After invoking this functions no handles for new groups
/// could be created. Accessing the existing handles by their
/// name will be lock-free from now on.
void freezeAllGroupHandles();

/// Free all group handles
///
/// Do not call while handles are in use!!! Mainly needed for testing.
///
/// Returns the number of groups.
int resetAllGroupHandles();

/// An opaque handle.
///
/// Instances should be passed by value.
///
/// TODO: Move into mixxx namespace.
class GroupHandle final {
    friend struct mixxx::grouphandle_private::GroupHandleImpl;
    friend int resetAllGroupHandles();

  public:
    static GroupHandle getOrCreateByName(
            const QString& name,
            bool create = true);

    static GroupHandle getByName(const QString& name) {
        return getOrCreateByName(name, false);
    }

    GroupHandle() = default;

    operator bool() const {
        return m_pImpl != nullptr;
    }

    /// Index
    ///
    /// 0-based integer identifier. The maximum number is limited by the
    /// total number of different descriptors, i.e. the total number of
    /// distinct group names.
    int index() const;

    /// Group name
    ///
    /// String identifier.
    QString name() const;

    friend bool operator<(GroupHandle lhs, GroupHandle rhs);
    friend bool operator==(GroupHandle lhs, GroupHandle rhs) {
        return lhs.m_pImpl == rhs.m_pImpl;
    }

    friend qhash_seed_t qHash(GroupHandle arg, qhash_seed_t seed = 0) {
        return arg.qHashImpl(seed);
    }

    friend QDebug operator<<(QDebug dbg, GroupHandle arg);

  private:
    explicit GroupHandle(mixxx::grouphandle_private::GroupHandleImpl* pImpl);

    qhash_seed_t qHashImpl(qhash_seed_t seed) const;

    mixxx::grouphandle_private::GroupHandleImpl* m_pImpl{};
};

inline bool operator!=(GroupHandle lhs, GroupHandle rhs) {
    return !(lhs == rhs);
}

Q_DECLARE_TYPEINFO(GroupHandle, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(GroupHandle)
