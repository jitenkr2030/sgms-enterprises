#pragma once

#include <QDialog>

class IndustrySelectorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit IndustrySelectorDialog(QWidget* parent = nullptr);

private:
    void buildUI();
    void selectIndustry(const QString& industry);
};
