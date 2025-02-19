// SPDX-FileCopyrightText: Copyright 2017 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <utility>
#include <QPixmap>
#include <QStandardItem>
#include <QStandardItemModel>
#include "common/common_types.h"

namespace Column {
enum List {
    GAME_NAME,
    ROOM_NAME,
    MEMBER,
    HOST,
    TOTAL,
};
}

class LobbyItem : public QStandardItem {
public:
    LobbyItem() = default;
    explicit LobbyItem(const QString& string) : QStandardItem(string) {}
    virtual ~LobbyItem() override = default;
};

class LobbyItemName : public LobbyItem {
public:
    static const int NameRole = Qt::UserRole + 1;
    static const int PasswordRole = Qt::UserRole + 2;

    LobbyItemName() = default;
    explicit LobbyItemName(bool has_password, QString name) : LobbyItem() {
        setData(name, NameRole);
        setData(has_password, PasswordRole);
    }

    QVariant data(int role) const override {
        if (role == Qt::DecorationRole) {
            bool has_password = data(PasswordRole).toBool();
            return has_password ? QIcon::fromTheme(QStringLiteral("lock")).pixmap(16) : QIcon();
        }
        if (role != Qt::DisplayRole) {
            return LobbyItem::data(role);
        }
        return data(NameRole).toString();
    }

    bool operator<(const QStandardItem& other) const override {
        return data(NameRole).toString().localeAwareCompare(other.data(NameRole).toString()) < 0;
    }
};

class LobbyItemDescription : public LobbyItem {
public:
    static const int DescriptionRole = Qt::UserRole + 1;

    LobbyItemDescription() = default;
    explicit LobbyItemDescription(QString description) {
        setData(description, DescriptionRole);
    }

    QVariant data(int role) const override {
        if (role != Qt::DisplayRole) {
            return LobbyItem::data(role);
        }
        auto description = data(DescriptionRole).toString();
        description.prepend(QStringLiteral("Description: "));
        return description;
    }

    bool operator<(const QStandardItem& other) const override {
        return data(DescriptionRole)
                   .toString()
                   .localeAwareCompare(other.data(DescriptionRole).toString()) < 0;
    }
};

class LobbyItemGame : public LobbyItem {
public:
    static const int TitleIDRole = Qt::UserRole + 1;
    static const int GameNameRole = Qt::UserRole + 2;
    static const int GameIconRole = Qt::UserRole + 3;

    LobbyItemGame() = default;
    explicit LobbyItemGame(u64 title_id, QString game_name, QPixmap smdh_icon) {
        setData(static_cast<unsigned long long>(title_id), TitleIDRole);
        setData(game_name, GameNameRole);
        if (!smdh_icon.isNull()) {
            setData(smdh_icon, GameIconRole);
        } else {
            setData(QIcon::fromTheme(QStringLiteral("chip")).pixmap(32), GameIconRole);
        }
    }

    QVariant data(int role) const override {
        if (role == Qt::DecorationRole) {
            auto val = data(GameIconRole);
            if (val.isValid()) {
                val = val.value<QPixmap>().scaled(32, 32, Qt::KeepAspectRatio,
                                                  Qt::TransformationMode::SmoothTransformation);
            } else {
                auto blank_image = QPixmap(32, 32);
                blank_image.fill(Qt::black);
                val = blank_image;
            }
            return val;
        } else if (role != Qt::DisplayRole) {
            return LobbyItem::data(role);
        }
        return data(GameNameRole).toString();
    }

    bool operator<(const QStandardItem& other) const override {
        return data(GameNameRole)
                   .toString()
                   .localeAwareCompare(other.data(GameNameRole).toString()) < 0;
    }
};

class LobbyItemHost : public LobbyItem {
public:
    static const int HostUsernameRole = Qt::UserRole + 1;
    static const int HostIPRole = Qt::UserRole + 2;
    static const int HostPortRole = Qt::UserRole + 3;
    static const int HostVerifyUIDRole = Qt::UserRole + 4;

    LobbyItemHost() = default;
    explicit LobbyItemHost(QString username, QString ip, u16 port, QString verify_uid) {
        setData(username, HostUsernameRole);
        setData(ip, HostIPRole);
        setData(port, HostPortRole);
        setData(verify_uid, HostVerifyUIDRole);
    }

    QVariant data(int role) const override {
        if (role != Qt::DisplayRole) {
            return LobbyItem::data(role);
        }
        return data(HostUsernameRole).toString();
    }

    bool operator<(const QStandardItem& other) const override {
        return data(HostUsernameRole)
                   .toString()
                   .localeAwareCompare(other.data(HostUsernameRole).toString()) < 0;
    }
};

class LobbyMember {
public:
    LobbyMember() = default;
    LobbyMember(const LobbyMember& other) = default;
    explicit LobbyMember(QString username_, QString nickname_, u64 title_id_, QString game_name_)
        : username(std::move(username_)), nickname(std::move(nickname_)), title_id(title_id_),
          game_name(std::move(game_name_)) {}
    ~LobbyMember() = default;

    QString GetName() const {
        if (username.isEmpty() || username == nickname) {
            return nickname;
        } else {
            return QStringLiteral("%1 (%2)").arg(nickname, username);
        }
    }
    u64 GetTitleId() const {
        return title_id;
    }
    QString GetGameName() const {
        return game_name;
    }

private:
    QString username;
    QString nickname;
    u64 title_id;
    QString game_name;
};

Q_DECLARE_METATYPE(LobbyMember);

class LobbyItemMemberList : public LobbyItem {
public:
    static const int MemberListRole = Qt::UserRole + 1;
    static const int MaxPlayerRole = Qt::UserRole + 2;

    LobbyItemMemberList() = default;
    explicit LobbyItemMemberList(QList<QVariant> members, u32 max_players) {
        setData(members, MemberListRole);
        setData(max_players, MaxPlayerRole);
    }

    QVariant data(int role) const override {
        switch (role) {
        case Qt::DisplayRole: {
            auto members = data(MemberListRole).toList();
            return QStringLiteral("%1 / %2 ")
                .arg(QString::number(members.size()), data(MaxPlayerRole).toString());
        }
        case Qt::ForegroundRole: {
            auto members = data(MemberListRole).toList();
            auto max_players = data(MaxPlayerRole).toInt();
            if (members.size() >= max_players) {
                return QBrush(QColor(255, 48, 32));
            } else if (members.size() == (max_players - 1)) {
                return QBrush(QColor(255, 140, 32));
            } else if (members.size() == 0) {
                return QBrush(QColor(128, 128, 128));
            }
            // FIXME: How to return a value that tells Qt not to modify the
            // text color from the default (as if Qt::ForegroundRole wasn't overridden)?
            return QBrush(nullptr);
        }
        default:
            return LobbyItem::data(role);
        }
    }

    bool operator<(const QStandardItem& other) const override {
        // sort by rooms that have the most players
        int left_members = data(MemberListRole).toList().size();
        int right_members = other.data(MemberListRole).toList().size();
        return left_members < right_members;
    }
};

/**
 * Member information for when a lobby is expanded in the UI
 */
class LobbyItemExpandedMemberList : public LobbyItem {
public:
    static const int MemberListRole = Qt::UserRole + 1;

    LobbyItemExpandedMemberList() = default;
    explicit LobbyItemExpandedMemberList(QList<QVariant> members) {
        setData(members, MemberListRole);
    }

    QVariant data(int role) const override {
        if (role != Qt::DisplayRole) {
            return LobbyItem::data(role);
        }
        auto members = data(MemberListRole).toList();
        QString out;
        bool first = true;
        for (const auto& member : members) {
            if (!first)
                out.append(QStringLiteral("\n"));
            const auto& m = member.value<LobbyMember>();
            if (m.GetGameName().isEmpty()) {
                out += QString(QObject::tr("%1 is not playing a game")).arg(m.GetName());
            } else {
                out += QString(QObject::tr("%1 is playing %2")).arg(m.GetName(), m.GetGameName());
            }
            first = false;
        }
        return out;
    }
};
