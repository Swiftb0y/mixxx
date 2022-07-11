#include <gtest/gtest.h>
#include <QtDebug>

#include "engine/channelhandle.h"
#include "test/mixxxtest.h"

TEST(ChannelHandleTest, GroupHandle) {
    resetAllGroupHandles();

    const QString group = "[Test]";
    const QString group2 = "[Test2]";

    EXPECT_FALSE(GroupHandle::getByName(group));
    EXPECT_EQ(0, GroupHandle::getOrCreateByName(group).index());
    EXPECT_EQ(0, GroupHandle::getOrCreateByName(group).index());
    GroupHandle testHandle = GroupHandle::getByName(group);
    EXPECT_TRUE(testHandle);
    EXPECT_EQ(0, testHandle.index());
    EXPECT_QSTRING_EQ(group, testHandle.name());
    EXPECT_EQ(testHandle, testHandle);

    EXPECT_FALSE(GroupHandle::getByName(group2));
    EXPECT_EQ(1, GroupHandle::getOrCreateByName(group2).index());
    EXPECT_EQ(1, GroupHandle::getOrCreateByName(group2).index());
    GroupHandle testHandle2 = GroupHandle::getByName(group2);
    EXPECT_TRUE(testHandle2);
    EXPECT_EQ(1, testHandle2.index());
    EXPECT_QSTRING_EQ(group2, testHandle2.name());
    EXPECT_EQ(testHandle2, testHandle2);

    EXPECT_NE(testHandle, testHandle2);
}

TEST(ChannelHandleTest, ChannelHandleMap) {
    resetAllGroupHandles();

    GroupHandle test = GroupHandle::getOrCreateByName("[Test]");
    GroupHandle test2 = GroupHandle::getOrCreateByName("[Test2]");

    ChannelHandleMap<QString> map;

    EXPECT_QSTRING_EQ(QString(), map.at({}));

    map.insert(test2, "bar");
    EXPECT_QSTRING_EQ("bar", map.at(test2));

    map.insert(test, "foo");
    EXPECT_QSTRING_EQ("foo", map.at(test));

    QString& reference = map[test];
    reference.chop(1);
    EXPECT_QSTRING_EQ("fo", map.at(test));

    // Replaces existing value.
    map.insert(test, "foo");
    EXPECT_QSTRING_EQ("foo", map.at(test));
}
