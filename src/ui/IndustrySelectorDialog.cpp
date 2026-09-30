#include "IndustrySelectorDialog.h"
#include "core/IndustryManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollArea>

IndustrySelectorDialog::IndustrySelectorDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Select Your Industry");
    setFixedSize(720, 600);
    setStyleSheet("QDialog { background: #0D1117; }");
    buildUI();
}

void IndustrySelectorDialog::buildUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(20);

    auto* titleLabel = new QLabel("Welcome to SGMS");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #D4AF37; background: transparent;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    auto* subtitleLabel = new QLabel("Select your industry to customize the application");
    subtitleLabel->setStyleSheet("font-size: 14px; color: #888; background: transparent;");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subtitleLabel);

    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    auto* gridWidget = new QWidget();
    gridWidget->setStyleSheet("background: transparent;");
    auto* grid = new QGridLayout(gridWidget);
    grid->setSpacing(15);

    QStringList industries = IndustryManager::availableIndustries();
    QStringList descriptions = {
        "Security guard agencies & services",
        "Cleaning & housekeeping agencies",
        "Domestic help & maid services",
        "Driver & transport agencies",
        "Multi-service facility management",
        "Cook, chef & catering services",
        "Bouncer & bodyguard services",
        "Pest control agencies",
        "Gardening & landscaping services",
        "Nanny & childcare agencies",
        "Patient care & attendants",
        "Warehouse & logistics staffing",
        "Construction labour supply",
        "Hotel & hospitality staffing",
        "Event & temporary staffing",
        "IT & technical staff augmentation",
        "Custom workforce management"
    };

    for (int i = 0; i < industries.size(); i++) {
        auto* btn = new QPushButton();
        btn->setCursor(Qt::PointingHandCursor);
        btn->setMinimumHeight(90);
        btn->setMaximumHeight(100);
        btn->setText(industries[i] + "\n" + descriptions[i]);
        btn->setStyleSheet(
            "QPushButton {"
            "  background: #1A1A2E;"
            "  border: 2px solid #2A2A3A;"
            "  border-radius: 12px;"
            "  padding: 12px 16px;"
            "  text-align: left;"
            "  color: #E0E0E0;"
            "  font-size: 13px;"
            "}"
            "QPushButton:hover {"
            "  border-color: #D4AF37;"
            "  background: #1E1E35;"
            "}"
        );

        QString ind = industries[i];
        connect(btn, &QPushButton::clicked, this, [this, ind]() {
            selectIndustry(ind);
        });

        int row = i / 2;
        int col = i % 2;
        grid->addWidget(btn, row, col);
    }

    scrollArea->setWidget(gridWidget);
    mainLayout->addWidget(scrollArea, 1);

    auto* footerLabel = new QLabel("You can change this later in Settings");
    footerLabel->setStyleSheet("font-size: 11px; color: #555; background: transparent;");
    footerLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(footerLabel);
}

void IndustrySelectorDialog::selectIndustry(const QString& industry)
{
    IndustryManager::instance().setIndustry(industry);
    accept();
}
