#pragma once

#include <QString>
#include <QStringList>
#include <QMap>

class IndustryManager
{
public:
    static IndustryManager& instance();

    QString industry() const;
    void setIndustry(const QString& industry);

    QString label(const QString& key) const;
    QMap<QString, QString> allLabels() const;

    bool isFirstLaunch() const;

    void save();
    void load();

    static QStringList availableIndustries();
    static QMap<QString, QString> labelsFor(const QString& industry);

private:
    IndustryManager();
    QString m_industry;
    QMap<QString, QString> m_labels;
};
