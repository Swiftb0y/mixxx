#include "control/grouphandle.h"

#include <QAtomicInt>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>

#include "util/compatibility/qatomic.h"

namespace {

constexpr int kInvalidIndex = -1;

QMutex allGroupHandlesByNameMutex;

typedef QHash<QString, GroupHandle> AllGroupHandlesByName;
AllGroupHandlesByName allGroupHandlesByName;

QAtomicInt allGroupHandlesFrozen;

} // namespace

namespace mixxx {
namespace grouphandle_private {

struct GroupHandleImpl final {
    GroupHandleImpl() = default;
    GroupHandleImpl(GroupHandleImpl&&) = delete;
    GroupHandleImpl(const GroupHandleImpl&) = delete;
    GroupHandleImpl& operator=(GroupHandleImpl&&) = delete;
    GroupHandleImpl& operator=(const GroupHandleImpl&) = delete;

    bool valid() const {
        DEBUG_ASSERT(m_index == kInvalidIndex || m_index >= 0);
        DEBUG_ASSERT((m_index == kInvalidIndex) == m_name.isEmpty());
        return m_index != kInvalidIndex;
    }

    GroupHandleImpl(int index, QString name)
            : m_index(index),
              m_name(std::move(name)) {
    }

    int m_index = kInvalidIndex;
    QString m_name;
};

inline bool operator<(const GroupHandleImpl& lhs, const GroupHandleImpl& rhs) {
    return lhs.m_index < rhs.m_index;
}

inline bool operator==(const GroupHandleImpl& lhs, const GroupHandleImpl& rhs) {
    return lhs.m_index == rhs.m_index;
}

inline bool operator!=(const GroupHandleImpl& lhs, const GroupHandleImpl& rhs) {
    return !(lhs == rhs);
}

inline qhash_seed_t qHash(const GroupHandleImpl& arg, qhash_seed_t seed = 0) {
    return ::qHash(arg.m_index, seed);
}

inline QDebug operator<<(QDebug dbg, const mixxx::grouphandle_private::GroupHandleImpl& arg) {
    return dbg << "GroupHandle{" << arg.m_index << arg.m_name << "}";
}

} // namespace grouphandle_private

} // namespace mixxx

GroupHandle GroupHandle::getOrCreateByName(const QString& name, bool create) {
    DEBUG_ASSERT(name == name.trimmed());
    if (atomicLoadRelaxed(allGroupHandlesFrozen)) {
        // lock-free
        DEBUG_ASSERT(!create);
        auto iter = allGroupHandlesByName.find(name);
        if (iter != allGroupHandlesByName.end()) {
            qDebug() << "Found existing group handle" << iter.value();
            return iter.value();
        }
    } else {
        // blocking
        auto locked = QMutexLocker(&allGroupHandlesByNameMutex);
        auto iter = allGroupHandlesByName.find(name);
        if (iter != allGroupHandlesByName.end()) {
            // Unlock mutex before logging (I/O)
            locked.unlock();
            qDebug() << "Found existing group handle" << iter.value();
            return iter.value();
        }
        if (create && !allGroupHandlesFrozen.loadAcquire()) {
            const auto index = static_cast<int>(allGroupHandlesByName.size());
            // Integer overflow is neither expected nor handled
            DEBUG_ASSERT(index >= 0);
            // This will leak memory, i.e. descriptors will never be
            // freed during a session until application shutdown.
            const auto groupHandle =
                    GroupHandle{
                            new mixxx::grouphandle_private::GroupHandleImpl{
                                    index, name}};
            allGroupHandlesByName.insert(name, groupHandle);
            // Unlock mutex before logging (I/O)
            locked.unlock();
            qInfo() << "Created new group handle" << groupHandle;
            return groupHandle;
        }
    }
    qWarning() << "Unknown group name" << name;
    return {};
}

void freezeAllGroupHandles() {
    const auto locked = QMutexLocker(&allGroupHandlesByNameMutex);
    allGroupHandlesByName.squeeze();
    allGroupHandlesFrozen.storeRelease(1);
}

int resetAllGroupHandles() {
    auto locked = QMutexLocker(&allGroupHandlesByNameMutex);
    AllGroupHandlesByName tmpGroupHandlesByName;
    tmpGroupHandlesByName.swap(allGroupHandlesByName);
    allGroupHandlesFrozen.storeRelease(0);
    locked.unlock();
    for (const auto& groupHandle : qAsConst(tmpGroupHandlesByName)) {
        qInfo() << "Deleting" << groupHandle;
        delete groupHandle.m_pImpl;
    }
    return tmpGroupHandlesByName.size();
}

GroupHandle::GroupHandle(mixxx::grouphandle_private::GroupHandleImpl* pImpl)
        : m_pImpl(pImpl) {
    DEBUG_ASSERT(!m_pImpl || m_pImpl->valid());
}

qhash_seed_t GroupHandle::qHashImpl(qhash_seed_t seed) const {
    if (m_pImpl) {
        return qHash(*m_pImpl, seed);
    } else {
        return qHash(mixxx::grouphandle_private::GroupHandleImpl{}, seed);
    }
}

bool operator<(GroupHandle lhs, GroupHandle rhs) {
    if (!rhs.m_pImpl) {
        return false;
    }
    if (!lhs.m_pImpl) {
        return true;
    }
    return *lhs.m_pImpl < *rhs.m_pImpl;
}

int GroupHandle::index() const {
    if (!m_pImpl) {
        return kInvalidIndex;
    }
    DEBUG_ASSERT(m_pImpl->valid());
    return m_pImpl->m_index;
}

QString GroupHandle::name() const {
    if (!m_pImpl) {
        return {};
    }
    DEBUG_ASSERT(m_pImpl->valid());
    return m_pImpl->m_name;
}

QDebug operator<<(QDebug dbg, GroupHandle arg) {
    if (arg.m_pImpl) {
        return dbg << *arg.m_pImpl;
    } else {
        return dbg << mixxx::grouphandle_private::GroupHandleImpl{};
    }
}
