#pragma once

#include <QtGlobal>

#include "util/assert.h"

template<class T>
class Singleton {
  public:
    static T* createInstance() {
        VERIFY_OR_DEBUG_ASSERT(!m_instance) {
            qWarning() << "Singleton class has already been created!";
            return m_instance;
        }

        m_instance = new T();
        return m_instance;
    }

    static T* instance() {
        VERIFY_OR_DEBUG_ASSERT(m_instance) {
            qWarning() << "Singleton class has not been created yet, returning nullptr";
        }
        return m_instance;
    }

    static void destroy() {
        VERIFY_OR_DEBUG_ASSERT(m_instance) {
            qWarning() << "Singleton class has already been destroyed!";
            return;
        }
        delete m_instance;
        m_instance = nullptr;
    }

    Singleton(const Singleton&) = delete;
    const Singleton& operator= (const Singleton&) = delete;

  private:
    Singleton() {
    }

    static T* m_instance;

    // required in order to call private constructors of the passed class.
    friend T;
};

template<class T>
T* Singleton<T>::m_instance = nullptr;
